/*
  Copyright (c) 2009-2017 Dave Gamble and cJSON contributors

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "sys.h"
#define 	FALSE 0
#define 	TRUE 1
#define 	DEBUG_APP_TASK 1


char* pszJsonText = "{\"result\":{\"id\":1,\"name\":\"aa\",\"datetime\":\"2016-04-25 16:38:54\",\"status\":1}, \
        \"code\":10000,\"message\":\"ok\",\"sign\":[0,1,2]}";//

typedef struct
{
	u32 cID;
	char *pName;
	char *datetime;
	u8 cStatus;
	u32 sign[3] ;
}SCAN_CODE_LIST_MEM_TypeDef;

SCAN_CODE_LIST_MEM_TypeDef g_stScanCodeListMem;

u8 ParseScanCodeJson(const char* pszJsonText)
{
    cJSON   *root, *result;
    u32 code=0;
    u8 ret = FALSE;

        //将json数据解析成json结构体
    root= cJSON_Parse(pszJsonText);

    if (root)
    { 
		//从根节点获取code元素的值 int型
			code = cJSON_GetObjectItem(root, "code")->valueint;

			if(code != 10001)
			{
				//从根节点获取result节点
				result = cJSON_GetObjectItem(root, "result");

				if (result)
				{ 
					//从result节点获取各个元素的值
					g_stScanCodeListMem.cID =  cJSON_GetObjectItem(result, "id")->valueint; //int 型
					g_stScanCodeListMem.pName = cJSON_GetObjectItem(result, "name")->valuestring;//字符串型
					g_stScanCodeListMem.datetime = cJSON_GetObjectItem(result, "datetime")->valuestring;//字符串型
					g_stScanCodeListMem.cStatus = cJSON_GetObjectItem(result, "status")->valueint;//int 型

					cJSON *taskArry=cJSON_GetObjectItem(result,"sign");//取数组
					int arrySize=cJSON_GetArraySize(taskArry);//数组大小
					printf(" %d ", arrySize); 

#ifdef DEBUG_APP_TASK        
					printf("\r\n");
					printf(" cID:%d ", g_stScanCodeListMem.cID);       
					printf(" pName:%s", g_stScanCodeListMem.pName);              
					printf(" datetime:%s ", g_stScanCodeListMem.datetime);
					printf(" cStatus:%d ", g_stScanCodeListMem.cStatus);

					printf("\r\n");
#endif                  
					ret = TRUE;
				}
			}

        //释放资源
        cJSON_Delete(root);
    }

    return ret;
}



/***************************************************************************/
//{
//    "message":"",
//    "allRowCount":1,
//    "root":[{
//            "value":"2011-03-17 11:15 - xxxx - xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
//            "id":"XXXX_20110315000"
//        }],
//    "success":true
//}

//   cJSON *jsonroot = cJSON_Parse(my_json_string); //json根
//    if(cJSON_GetObjectItem(jsonroot,"success")->valuestring=="false")
//    {
//        packValue.ack=0;
//        return packValue;
//    }
//    int taskNum = cJSON_GetObjectItem(jsonroot,"allRowCount")->valueint;
//    cJSON *taskArry=cJSON_GetObjectItem(jsonroot,"root");//取数组
//    int arrySize=cJSON_GetArraySize(taskArry);//数组大小
//    cJSON *tasklist=taskArry->child;//子对象
//    while(tasklist!=NULL)
//    {
//                printf("%s\n",cJSON_GetObjectItem(tasklist,"value")->valuestring);
//        tasklist=tasklist->next;
//    }

