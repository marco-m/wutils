/* Copyright (c) 2014, Marco Molteni.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef URLROUTER_HPP
#define URLROUTER_HPP

#include <Wt/WObject>
#include <Wt/WApplication>

#include <regex>
#include <functional>
#include <string>
#include <vector>
#include <utility>
#include <stdexcept>
#include <sstream>

namespace Wt {
class WApplication;
}

namespace WU {

// Needed because some Wt include file brings in boost::placeholders clash.
namespace sph = std::placeholders;

// From Scott Meyers, "Effective C++".
class Uncopyable {
protected:
    Uncopyable() {}
    ~Uncopyable() {}
private:
    Uncopyable(const Uncopyable&);
    Uncopyable& operator=(const Uncopyable&);
};

class UrlRouterError : public std::runtime_error
{
public:
    explicit UrlRouterError(const std::string& what)
        : std::runtime_error(what)
    { }
};

//urlpatterns = patterns('',
//    url(r'^articles/2003/$',                  'news.views.specialcase2003'),
//    url(r'^articles/(\d{4})/$',               'news.views.yeararchive'),
//    url(r'^articles/(\d{4})/(\d{2})/$',       'news.views.montharchive'),
//    url(r'^articles/(\d{4})/(\d{2})/(\d+)/$', 'news.views.articledetail'),
//)
//To capture a value from the URL, just put parenthesis around it.
//A request to /articles/2005/03/ would match the third entry in the list.
//Django would call the function
//news.views.month_archive(request, '2005', '03').
//Error handling
//handler404
/**
 * A regex-based URL router for Wt inspired by the smart Django "URLconf".
 * See https://docs.djangoproject.com/en/dev/topics/http/urls
 *
 * It reacts to the internalPathChanged() signal of the Wt::WApplication
 * set as its parent and routes to the **first** matching handler, so the
 * **order** of the calls to add() is important.
 *
 * The regex string must be a valid std::regex() regex.
 *
 * Since its ownership is transferred to the current WApplication, it must be
 * created on the heap.
 *
 * Usage example:
 *
 * void specialcase2003();
 * void yeararchive(const std::string& arg1);
 * void montharchive(const std::string& arg1, const std::string& arg2);
 * void articledetail(const std::string& arg1, const std::string& arg2, const std::string& arg3);
 *
 * UrlRouter* r = new UrlRouter(Wt::WApplication::instance());
 *
 * r->add("/articles/2003/",                     this, specialcase2003);
 * r->add("/articles/(\\d{4})/",                 this, yeararchive);
 * r->add("/articles/(\\d{4})/(\\d{2})/",        this, montharchive);
 * r->add("/articles/(\\d{4})/(\\d{2})/(\\d+)/", this, articledetail);
 *
 * To capture a value from the URL, put a grouping parenthesis around it.
 *
 * Unfortunately, compared to the Python case, the patterns are uglier due
 * to C++ strings, that require to escape all backslashes.
 * Or, you can use C++11 raw literals, that avoid escaping backslashes at the
 * cost of adding confusion because they require an additional set of
 * parenthesis that are _not_ part of the regex:
 *
 * r->add(R"(/articles/2003/)",                  specialcase2003);
 * r->add(R"(/articles/(\d{4})/)",               yeararchive);
 * r->add(R"(/articles/(\d{4})/(\d{2})/)",       montharchive);
 * r->add(R"(/articles/(\d{4})/(\d{2})/(\d+)/)", articledetail);
 *
 * Captured arguments are always strings, no matter what kind of match the
 * regex does; it is up to the handler to do proper conversion if needed.
 *
 * Note: a mismatch between the number of groups declared in the regex and the
 * number of arguments accepted by the handler can only be detected at
 * runtime.
 */
class UrlRouter : public Wt::WObject, private Uncopyable
{
public:
    /**
     * Construct an UrlRouter object. It must be created on the heap.
     * @param parent The current WApplication, to which ownership will be
     *               transferred. Must be non-null.
     */
    UrlRouter(Wt::WApplication* parent)
        : Wt::WObject(parent)
    {
        assert(parent != 0);
        parent->internalPathChanged()
                .connect(this, &UrlRouter::onInternalPathChange);
    }

    ~UrlRouter() {}

    template<typename T>
    void add(const std::string& pattern, T *obj,
             void (T::* pm)())
    {
        std::regex regex(pattern);
        check_groups_args(regex.mark_count(), 0);
        _handlers0.push_back(std::bind(pm, obj));
        _regex2handler.push_back(std::make_pair(regex, _handlers0.size() - 1));
    }

    template<typename T>
    void add(const std::string& pattern, T *obj,
             void (T::* pm)(const std::string& arg1))
    {
        std::regex regex(pattern);
        check_groups_args(regex.mark_count(), 1);
        _handlers1.push_back(std::bind(pm, obj, sph::_1));
        _regex2handler.push_back(std::make_pair(regex, _handlers1.size() - 1));
    }

    template<typename T>
    void add(const std::string& pattern, T *obj,
             void (T::* pm)(const std::string& arg1,
                            const std::string& arg2))
    {
        std::regex regex(pattern);
        check_groups_args(regex.mark_count(), 2);
        _handlers2.push_back(std::bind(pm, obj, sph::_1, sph::_2));
        _regex2handler.push_back(std::make_pair(regex, _handlers2.size() - 1));
    }

    template<typename T>
    void add(const std::string& pattern, T *obj,
             void (T::* pm)(const std::string& arg1,
                            const std::string& arg2,
                            const std::string& arg3))
    {
        std::regex regex(pattern);
        check_groups_args(regex.mark_count(), 3);
        _handlers3.push_back(std::bind(pm, obj, sph::_1, sph::_2, sph::_3));
        _regex2handler.push_back(std::make_pair(regex, _handlers3.size() - 1));
    }


private:
    void onInternalPathChange(const std::string& path);

    void check_groups_args(int groups, int args);


    // Map a regex to the index in the _handlersN vector to find the
    // corresponding handler.
    std::vector<std::pair<std::regex, int> > _regex2handler;

    typedef std::function<void ()>                        handler0_t;
    typedef std::function<void (const std::string& arg1)> handler1_t;
    typedef std::function<void (const std::string& arg1,
                                const std::string& arg2)> handler2_t;
    typedef std::function<void (const std::string& arg1,
                                const std::string& arg2,
                                const std::string& arg3)> handler3_t;

    std::vector<handler0_t> _handlers0;
    std::vector<handler1_t> _handlers1;
    std::vector<handler2_t> _handlers2;
    std::vector<handler3_t> _handlers3;

    std::ostringstream _oss;
};

}

#endif // URLROUTER_HPP
