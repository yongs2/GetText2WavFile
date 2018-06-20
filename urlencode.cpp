#include "urlencode.h"

std::string UrlEncode(const std::string &s)
{
    //RFC 3986 section 2.3 Unreserved Characters (January 2005)
    // 참고: http://ko.wikipedia.org/wiki/퍼센트_인코딩
    const std::string unreserved = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.~";

    std::string escaped="";
    for(size_t i=0; i<s.length(); i++)
    {
        if (unreserved.find_first_of(s[i]) != std::string::npos)
        {
            escaped.push_back(s[i]);
        }
        else
        {
            escaped.append("%");
            char buf[3];

            /* char를 unsigned char로 바꿔주어야 합니다.
             * (그렇지 않으면 한글 등의 문자를 제대로 변환하지 못합니다.)
             */
            sprintf(buf, "%.2X", (unsigned char) s[i]);
            escaped.append(buf);
        }
    }
    return escaped;
}
