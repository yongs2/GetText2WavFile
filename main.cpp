#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <stdarg.h>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <sstream>
#include <vector>
#include "iconvpp.h"
#include "coucheHttp.h"
#include "rechercheInformation.h"
#include "regexec.h"
#include "urlencode.h"
#include "string_util.h"

std::string ConvertSWF2WAV(std::string szSwfFile);
std::string GetTTSFile(std::string szText);


int main(int argc, char *argv[])
{
    std::string ttsFile = "", wavFile = "";
    std::string line;
    std::string page;

    if(argc <= 1)
    {
        ttsFile = GetTTSFile("안녕하세요, 문장을 음성으로 변환하는 테스트를 진행하고 있습니다. 부드럽게 들리는 지 확인해 주세요.");
        wavFile = ConvertSWF2WAV(ttsFile);
    }
    else if(argc > 1)
    {
        std::string szInput = argv[1];
        std::string out;
        iconvpp::converter conv("UTF-8", "EUC-KR", true, 2048);
        conv.convert(szInput, out);

        ttsFile = GetTTSFile(out);
        wavFile = ConvertSWF2WAV(ttsFile);
    }

    return 0;
}

std::string ConvertSWF2WAV(std::string szSwfFile)
{
    std::string szWavFile = "tts.wav";
    FILE* pipe = NULL;
    char szCmd[256] = "\0";

    snprintf(szCmd, sizeof(szCmd)-1, "ffmpeg -i %s -ar 8000 -ac 1 -acodec pcm_mulaw -y %s", szSwfFile.c_str(), szWavFile.c_str());

    pipe = popen(szCmd, "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while(!feof(pipe)) {
        if(fgets(buffer, 128, pipe) != NULL)
        result += buffer;
    }
    pclose(pipe);

    printf("Cmd[%s], Result=%s\n", szCmd, result.c_str());
    return szWavFile;
}

std::string GetTTSFile(std::string szText)
{
    Header head;
    std::string szUrlEncodeText = szText;
    std::string page;
    std::string ttsFile = "test";

    // Common
    head.setAccept("text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8");
    head.setUserAgent("Mozilla/5.0 (Windows NT 6.1; WOW64; rv:21.0) Gecko/20100101 Firefox/21.0");
    head.setAccept_language("ko-KR,ko;q=0.8,en-US;q=0.6,en;q=0.4");
    head.setHost("http://text-to-speech.imtranslator.net");

    // 1st
    head.setPath("/speech.asp?dir=ko");
    //head.addCookie("TTSid","427096821");
    head.setMethod("GET");

    Http http1(head);   
    page = http1.getPage(&head);
    //std::cout<<page<<std::endl;

    // 2nd 
    szUrlEncodeText = UrlEncode(szText);
    std::cout << "(" << szText << "), URLEncode=" << szUrlEncodeText << "\n" ;

    head.setPath("/sockets/box.asp");
    head.setMethod("POST");
    head.addVariable("text", szUrlEncodeText);

    Http http2(head);
    page = http2.getPage(&head);
    //std::cout<<page<<std::endl;

    std::string ttsId;
    std::vector<std::string> cookies = split(head.getCookie(), ' ');
    for ( int i = 0; i < cookies.size(); i++) {
        int nFind = cookies[i].find("TTSid=");
        std::cout << "Cookie=[" << cookies[i] << "]=" << nFind << std::endl;
        if (nFind >= 0) {
            ttsId = cookies[i];
            RechercheInfo::searchCutLeft(&ttsId,"TTSid=",true);
            RechercheInfo::searchCutRight(&ttsId,";",true);
            trim(ttsId);
            std::cout << "ttsId=[" << ttsId << "]" << std::endl;
        }
    }

    // 3rd
    std::string path = "/sockets/tts.asp?speed=0&url=F_TR&dir=ko&B=1&ID=" + ttsId + "&chr=MiaHead&vc=VW%%20Yumi&FA=1";
    std::cout << "path=[" << path << "]" << std::endl;
    head.setPath(path);
    head.setMethod("GET");

    Http http3(head);   
    page = http3.getPage(&head);
    //std::cout<<page<<std::endl;

    // 4th
    std::string regex_pattern = "(SRC=\")([A-Za-z0-9:._?=/]*)(\")";

    int nRet = 0;
    char szData[3][128] = {"\0", "\0", "\0"};
    nRet = ChkRegex(regex_pattern.c_str(), page.c_str(), 3, &szData[0], &szData[1], &szData[2]);    // Group 이 3개 이므로, 그중 2번째 그룹 정보 추출
    std::cout << nRet << "URL=" << szData[1] << " ------------------------------------------------ <<<<\n";
    if(nRet <= 0) 
    {
        printf("ERROR, NOT URL\n");
    }
    else 
    {
        printf("\n\n\n");
        // URL 에서 Host 와 Path 분리
        char szHost[128] = "\0";
        char szPath[128] = "\0";
        std::string regex_pattern = "(http[s]*://[A-Za-z0-9.:]*)([A-Za-z0-9._?=/]*)";
        nRet = ChkRegex(regex_pattern.c_str(), szData[1], 2, szHost, szPath);    // Group 이 3개 이므로, 그중 2번째 그룹 정보 추출
        std::cout << nRet << "Host=" << szHost << " Path=" << szPath << " ------------------------------------------------ <<<<\n";
        if(nRet <= 0) 
        {
            printf("ERROR, NOT HOST and PATH\n");
        }
        else 
        {   // :80 으로 추가된 부분은 setHost 호출시 삭제해야 함
            char *pszColon = rindex(szHost, ':');
            printf("Host=%p, pszColon=%p\n", szHost, pszColon);
            if(pszColon != NULL)
            {
                szHost[pszColon - szHost] = '\0';
            }
            printf("Host=[%s]\n", szHost);

            head.setPath(szPath);
            head.setHost(szHost);
            head.setMethod("GET");

            Http http4(head);   
            page = http4.getPage(&head);
            int taille = page.length();
            std::cout<<"Page.length=" << page.length() <<std::endl;

            int totalLen = page.length();
            std::cout<<"Page.length=" << totalLen << ", [" << page.substr(page.length()-10, page.length()) << "]" <<std::endl;

            //std::string header;
            int nCRLF = page.find("\r\n\r\n");

            std::string header = page.substr(0, nCRLF+2);
            std::string body = page.substr(nCRLF+4, totalLen-nCRLF-4);

            std::string contentType = header;
            RechercheInfo::searchCutLeft(&contentType,"Content-Type:",true);
            RechercheInfo::searchCutRight(&contentType,"\r",true);
            trim(contentType);

            std::string contentLength = header;
            RechercheInfo::searchCutLeft(&contentLength,"Content-Length:",true);
            RechercheInfo::searchCutRight(&contentLength,"\r",true);
            trim(contentLength);

            std::cout<<"header=[" << header << "]=" << header.length() << std::endl;
            std::cout<<"contentType=[" << contentType << "]=" << contentType.length() << std::endl;
            std::cout<<"contentLength=[" << contentLength << "]=" << contentLength.length() << std::endl;
            std::cout<<"body=[" << body.substr(body.length()-10, body.length()) << "]=" << body.length() << std::endl;

            ttsFile = "tts.swf";
            std::ofstream file1(ttsFile, std::ios::out | std::ios::binary);
            if (file1.good())
            {
                file1.write(body.c_str(), body.size());
                file1.close();
            }
            else {
                std::cout << "file error write" << std::endl;
            }
        }
    }

    // memory clear
    head.removeVariable();
    head.removeCookie();

    return ttsFile;
}
