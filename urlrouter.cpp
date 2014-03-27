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

namespace WU {

void UrlRouter::onInternalPathChange(const std::string& path)
{
    /*
     * Iterate through the regexes in the same order as they have been added
     * with add(). First match wins.
     */
    for (std::pair<std::regex, int>& p : _regex2handler) {
        std::regex& re = p.first;
        int index = p.second;
        std::smatch sm;
        if (std::regex_match(path, sm, re)) {

            // Ugly, repetitive code because I don't know enough C++-fu...

            int nArgs = re.mark_count();
            if (nArgs == 0) {
                handler0_t f = _handlers0[index];
                f();
            } else if (nArgs == 1) {
                handler1_t f = _handlers1[index];
                f(sm[1].str());
            } else if (nArgs == 2) {
                handler2_t f = _handlers2[index];
                f(sm[1].str(), sm[2].str());
            } else if (nArgs == 3) {
                handler3_t f = _handlers3[index];
                f(sm[1].str(), sm[2].str(), sm[3].str());
            } else {
                assert(0 && "nArgs too big");
            }
            break;
        }
    }
}

void UrlRouter::check_groups_args(int groups, int args)
{
    if (groups != args) {
        _oss << "regex groups: " << groups << " but args: " << args;
        throw UrlRouterError(_oss.str());
    }
}

}
