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

#include "urlrouter.hpp"

#include <Wt/WApplication>
#include <Wt/Test/WTestEnvironment>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using testing::_;

/*
 * This is a dummy class containing handlers for the various Urls.
 * In production, it would probably inherit from Wt::WObject.
 *
 * Since the method UrlRouter::add() is template-based, there is no
 * need for this class to have virtual methods (as is normally required by
 * googlemock).
 */
class DummyView
{
public:
    void handler0a() { }
    void handler0b() { }
    void handler1(const std::string& arg1) { }
    void handler2(const std::string& arg1,
                  const std::string& arg2) { }
    void handler3(const std::string& arg1,
                  const std::string& arg2,
                  const std::string& arg3) { }
};

class Mock : public DummyView
{
public:
    MOCK_METHOD0(handler0a, void ());
    MOCK_METHOD0(handler0b, void ());
    MOCK_METHOD1(handler1, void (const std::string&));
    MOCK_METHOD2(handler2, void (const std::string&,
                                 const std::string&));
    MOCK_METHOD3(handler3, void (const std::string&,
                                 const std::string&,
                                 const std::string&));
};

// Test fixture.
class UrlRouterTest : public ::testing::Test
{
protected:
    virtual void SetUp() {
        _env = new Wt::Test::WTestEnvironment("/", "./wt_config.xml");
        _app = new Wt::WApplication(*_env);
        _mv = new Mock();
        _r = new WU::UrlRouter<Mock>(_app, _mv);
    }
    virtual void TearDown() {
        // _r is deleted by _app.
        delete _mv;
        delete _app;
        delete _env;
    }
    Wt::Test::WTestEnvironment* _env;
    Wt::WApplication*           _app;
    Mock*                       _mv;
    WU::UrlRouter<Mock>*        _r;
};


TEST_F(UrlRouterTest, EmptyRegex) {
    EXPECT_NO_THROW(_r->add("", &Mock::handler0a));
    EXPECT_THROW(_r->add("", &Mock::handler1), WU::UrlRouterError);
    EXPECT_THROW(_r->add("", &Mock::handler2), WU::UrlRouterError);
}

TEST_F(UrlRouterTest, OneGroup) {
    EXPECT_THROW(_r->add("()", &Mock::handler0a), WU::UrlRouterError);
    EXPECT_NO_THROW(_r->add("()", &Mock::handler1));
    EXPECT_THROW(_r->add("()", &Mock::handler2), WU::UrlRouterError);
}

TEST_F(UrlRouterTest, TwoGroups) {
    EXPECT_THROW(_r->add("()()", &Mock::handler0a), WU::UrlRouterError);
    EXPECT_THROW(_r->add("()()", &Mock::handler1), WU::UrlRouterError);
    EXPECT_NO_THROW(_r->add("()()", &Mock::handler2));
}

TEST_F(UrlRouterTest, Match0Groups1Match) {
    _r->add("/pippo", &Mock::handler0a);
    EXPECT_CALL(*_mv, handler0a()).Times(1);
    EXPECT_CALL(*_mv, handler1(_)).Times(0);
    EXPECT_CALL(*_mv, handler2(_, _)).Times(0);
    _app->setInternalPath("/pippo", true);
}

TEST_F(UrlRouterTest, Match0GroupsNMatches) {
    _r->add("/pippo", &Mock::handler0a);
    _r->add("/pluto", &Mock::handler0b);
    _r->add("/paperino", &Mock::handler0a);
    EXPECT_CALL(*_mv, handler0a()).Times(2);
    EXPECT_CALL(*_mv, handler0b()).Times(1);
    EXPECT_CALL(*_mv, handler1(_)).Times(0);
    EXPECT_CALL(*_mv, handler2(_, _)).Times(0);
    _app->setInternalPath("/pluto", true);
    _app->setInternalPath("/pippo", true);
    _app->setInternalPath("/paperino", true);
}

TEST_F(UrlRouterTest, NoMatch) {
    _r->add("/pippo", &Mock::handler0a);
    _r->add("/pluto", &Mock::handler0b);
    _r->add("/foo/(bar)", &Mock::handler1);
    _r->add("/(foo)/(bar)", &Mock::handler2);
    EXPECT_CALL(*_mv, handler0a()).Times(0);
    EXPECT_CALL(*_mv, handler0b()).Times(0);
    EXPECT_CALL(*_mv, handler1(_)).Times(0);
    EXPECT_CALL(*_mv, handler2(_, _)).Times(0);
    _app->setInternalPath("/foo", true);
    _app->setInternalPath("/bar", true);
    _app->setInternalPath("/p/ippo", true);
    _app->setInternalPath("/pluto/", true);
}

TEST_F(UrlRouterTest, SomeMatchesAsPython) {
    // C++11 raw literals are good (no need to escape backslash \) but
    // confusing for regex: they add another set of parenthesis.
    _r->add(R"(/articles/(\d{4})/)",               &Mock::handler1);
    _r->add(R"(/articles/(\d{4})/(\d{2})/)",       &Mock::handler2);
    _r->add(R"(/articles/(\d{4})/(\d{2})/(\d+)/)", &Mock::handler3);
    EXPECT_CALL(*_mv, handler1("1999")).Times(1);
    EXPECT_CALL(*_mv, handler1("1984")).Times(1);
    EXPECT_CALL(*_mv, handler2("1999", "04")).Times(1);
    EXPECT_CALL(*_mv, handler2("1984", "12")).Times(1);
    EXPECT_CALL(*_mv, handler3("2014", "03", "1")).Times(1);
    EXPECT_CALL(*_mv, handler3("2014", "03", "1234")).Times(1);
    EXPECT_CALL(*_mv, handler3("2014", "03", "-1")).Times(0);
    _app->setInternalPath("/articles/1999/", true);
    _app->setInternalPath("/articles/1999/04/", true);
    _app->setInternalPath("/articles/1984/", true);
    _app->setInternalPath("/articles/1984/12/", true);
    _app->setInternalPath("/articles/2014/03/1/", true);
    _app->setInternalPath("/articles/2014/03/1234/", true);
    _app->setInternalPath("/articles/2014/03/1234/-1/", true);
}

//TEST_F(UrlRouterTest, FirstMatchWins) {
//    MockViewer mv;
//    _r->add("/pippo", &mv, &Mocker::handler0);
//}

//TEST_F(UrlRouterTest, PathTooLong)
