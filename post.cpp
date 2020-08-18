#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <jsoncpp/json/json.h>
#include <iostream>
#include <sstream>
using namespace std;

#define POSTURL    "http://47.112.135.212:8084/login"
//#define POSTFIELDS "{\"username\":\"yanqingwei\",\"password\":\"123456\"}"
   
#define FILENAME   "curlposttest.log"
 
// size_t write_data(void* buffer,size_t size,size_t nmemb,void *stream)
// {
// 	cout << "write_data" << endl;
// 	FILE *fptr = (FILE*)stream;
// 	fwrite(buffer,size,nmemb,fptr);
// 	return size*nmemb;
// }
size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) 
{
    string data((const char*) ptr, (size_t) size * nmemb);

    *((stringstream*) stream) << data << endl;

    return size * nmemb;
}

int main(int argc,char *argv[])
{
	
	CURL *curl;
	CURLcode res;
	FILE* fptr;
	struct curl_slist *http_header = NULL;
    //以写的方式打开文件
	if ((fptr = fopen(FILENAME,"w")) == NULL)
	{
		//向文件中记录日志,没打开之前的记录要加文件路径FILENAME
		fprintf(stderr,"fopen file error:%s\n",FILENAME);
		return -1;
	}
 
	curl = curl_easy_init();
	if (!curl)
	{
		//打开后直接记录文件
		fprintf(stderr,"curl init failed\n");
		return -1;
	}
    //支持此方式
    //char url[100] = "http://47.112.135.212:8084/login";
	char param[500] = {0};
    stringstream out; 
    //构造json格式的参数
    Json::Value item;
    item["username"] = Json::Value("yanqingwei");
	item["password"] = Json::Value("123456");
    std::string str = item.toStyledString();
	//c_str()作为string类的成员函数,返回string的字符串首地址
    strcpy(param, str.c_str());
	//curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(param));
	//curl_easy_setopt(curl, CURLOPT_POST, 1);
	//curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
	//curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
	//curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);
	//curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
    // curl_easy_setopt(curl, CURLOPT_HEADER, 0L);
	//设置post的body为json类型,不然会报400 Bad Request
	struct curl_slist *pList = NULL; 
    pList = curl_slist_append(pList,"Content-Type: application/json;charset=utf-8");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, pList);
	curl_easy_setopt(curl,CURLOPT_URL,POSTURL); //url地址
	curl_easy_setopt(curl,CURLOPT_POSTFIELDS,param); //post参数,即post的body
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,write_data); //对返回的数据进行操作的函数地址
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,&out); //这是write_data的第四个参数值
	curl_easy_setopt(curl,CURLOPT_POST,1); //设置问非0表示本次操作为post
	//curl_easy_setopt(curl,CURLOPT_VERBOSE,1); //打印调试信息
	//curl_easy_setopt(curl,CURLOPT_HEADER,1); //将响应头信息和相应体一起传给write_data
	curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1); //设置为非0,响应头信息location
	curl_easy_setopt(curl,CURLOPT_COOKIE,"UDSESSIONID=yanqingwei:fed96a6a-17e2-462d-be3d-9c7eaab62511");
    //设置完属性执行post请求
	res = curl_easy_perform(curl);
	if (res != CURLE_OK)
	{
		switch(res)
		{
			case CURLE_UNSUPPORTED_PROTOCOL:
				fprintf(stderr,"不支持的协议,由URL的头部指定\n");
			case CURLE_COULDNT_CONNECT:
				fprintf(stderr,"不能连接到remote主机或者代理\n");
			case CURLE_HTTP_RETURNED_ERROR:
				fprintf(stderr,"http返回错误\n");
			case CURLE_READ_ERROR:
				fprintf(stderr,"读本地文件错误\n");
			default:
				fprintf(stderr,"返回值:%d\n",res);
		}
		return -1;
	}

	string str_json = out.str();
	cout << str_json << endl;
	Json::Reader reader;
    Json::Value value;
    if(reader.parse(str_json, value))
    {
        cout << "parse is ok" << endl;
        if(!value["Data"].isNull())
        {
            Json::Value value_2 = value["Data"];
			string myCookie = value_2["Token"].asString();
			myCookie = "UDSESSIONID=" + myCookie;
            cout << "Token:" << myCookie << endl;
            
        }
        else
        {
         cout << "*******Longitude is NULL*******" << endl;
        }
	}
	else
	{
		cout << "parse is error" << endl;
	}
	
	//清除描述字符curl
	curl_easy_cleanup(curl);
}
