

#include "s2j.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "MySys.h"
#include <stdlib.h>
typedef struct {
    char name[16];
} Hometown;

typedef struct {
    uint8_t id;
    double weight;
    uint8_t score[8];
    char name[10];
    Hometown hometown;
} Student;

/**
 * Student JSON object to structure object
 *
 * @param json_obj JSON object
 *
 * @return structure object
 */
static void *json_to_struct(cJSON* json_obj) {
//    /* create Student structure object */
//    s2j_create_struct_obj(struct_student, Student);

//    /* deserialize data to Student structure object. */
//    s2j_struct_get_basic_element(struct_student, json_obj, int, id);
//    s2j_struct_get_array_element(struct_student, json_obj, int, score);
//    s2j_struct_get_basic_element(struct_student, json_obj, string, name);
//    s2j_struct_get_basic_element(struct_student, json_obj, double, weight);

//    /* deserialize data to Student.Hometown structure object. */
//    s2j_struct_get_struct_element(struct_hometown, struct_student, json_hometown, json_obj, Hometown, hometown);
//    s2j_struct_get_basic_element(struct_hometown, json_hometown, string, name);

//    /* return Student structure object pointer */
//    return struct_student;
}

/**
 * Student structure object to JSON object
 *
 * @param struct_obj structure object
 *
 * @param JSON object
 */
static cJSON *struct_to_json(void* struct_obj) {
//    Student *struct_student = (Student *)struct_obj;

//    /* create Student JSON object */
//    s2j_create_json_obj(json_student);

//    /* serialize data to Student JSON object. */
//    s2j_json_set_basic_element(json_student, struct_student, int, id);
//    s2j_json_set_basic_element(json_student, struct_student, double, weight);
//    s2j_json_set_array_element(json_student, struct_student, int, score, 8);
//    s2j_json_set_basic_element(json_student, struct_student, string, name);

//    /* serialize data to Student.Hometown JSON object. */
//    s2j_json_set_struct_element(json_hometown, json_student, struct_hometown, struct_student, Hometown, hometown);
//    s2j_json_set_basic_element(json_hometown, struct_hometown, string, name);

//    /* return Student JSON object pointer */
//    return json_student;
}
/*
cJSON *json_student = struct_to_json(&orignal_student_obj);//结构体转字json结构体
char *json_text=cJSON_Print(json_student);//将json结构体转成字符串
cJSON *char_json=cJSON_Parse(cJSON_Print(json_student)); //json字符串转json结构体
Student * converted_student_obj = json_to_struct(json_student);//json结构体转本地结构体

*/

int json_test(void) {
	static int num=0;
	uint16_t  i=0;
	char data_bufer[200];
	cJSON *json_student=0;
	char *p=0;
    static Student orignal_student_obj = {
			.id = 24,
            .weight = 71.2,
            .score = {1, 2, 3, 4, 5, 6, 7, 8},
            .name = "armink",
            .hometown.name = "China",
    };
	orignal_student_obj.id=num++;
	orignal_student_obj.weight=num++;
	if(num==300)
		num=0;
    /* serialize Student structure object */
    json_student = struct_to_json(&orignal_student_obj);//结构体转字json结构体
	
	p=cJSON_PrintUnformatted(json_student);//返回的是JSON结构的字符串
	
	s2j_delete_json_obj(json_student);//在第二次使用json_student之前要把json结构体清除一次
	printf("%s",p); //把json结构体转换成字符串
	json_student=cJSON_Parse(p);//将字符串转换成json结构体
	

	
    /* deserialize Student structure object */
    Student * converted_student_obj = json_to_struct(json_student);//json结构体转结构体

    /* compare Student structure object */
    if(memcmp(&orignal_student_obj, converted_student_obj, sizeof(Student))) {
        printf("Converted failed!\n");
    } else {
		printf("id %d\r\n",converted_student_obj->id);
		printf("weight %.2f\r\n",converted_student_obj->weight);
        printf("Converted OK!\n");
    }
	free(p); 
    s2j_delete_json_obj(json_student);
    s2j_delete_struct_obj(converted_student_obj);

    return 0;
}
#include "me3630.h"
#include "debug.h"
/*
将发送给服务的结构体转换成JSON结构体
：转换给服务器的数据头
	int ApiLogo;			//api标识码，由服务器给出，占用
	int	RequestID;			//请求ID，由客户端给出，服务器在接收到数据后会返回已接受请求的西响应
	int DataLen;			//数据长度
	int	DataOutWay;			//数据序列化方式 1 msgpack 序列化
	int PackageInsideNum;	//数据在分包内的序列，从0开始
	int	PackageNum;			//数据的分包数量，最小为1
*/

/*
GGA打包成JSON数据
GgaData:传入的GGA的字符数据
rturn_data：反馈会打包好额度JSON的数据
*/
#define TEST_GGA 1
void cJSON_GgaToJson(char* GgaData)
{
	char *DataBuf;
	int SendLen=0;
	char* pGGAJson=0;
#if TEST_GGA
	static uint16_t TestRequestID=1;
#endif
	stMe3630SendHead DataHead;//数据的头
	s2j_create_json_obj(JsonData);
	DataHead.ApiLogo=sw32_uint32(eApiLogoGGA);	
#if TEST_GGA
	if(TestRequestID<500)
	{
		TestRequestID++;
		DataHead.RequestID=sw32_uint32(TestRequestID);
	}else{
		TestRequestID=1;
	}		
#endif
	DataHead.DataOutWay=sw32_uint32(eDataOutWayJson);		
	DataHead.PackageInsideNum=0;
	DataHead.PackageNum=sw32_uint32(1);		
	
	cJSON_AddStringToObject(JsonData,"GGA",GgaData);//将GGA的字符串转换成JSON结构的数据
	pGGAJson =cJSON_PrintUnformatted(JsonData);//返回的是JSON结构的字符串
	
	
	DataHead.DataLen=sw32_uint32(strlen(pGGAJson));	
	SendLen=strlen(pGGAJson)+sizeof(stMe3630SendHead);
	
	DataBuf=(char*)malloc(SendLen);
	memcpy(DataBuf, (char *)(&DataHead), sizeof(stMe3630SendHead));
//	strncpy(DataBuf,(char *)(&DataHead),sizeof(stMe3630SendHead));
	strncpy(DataBuf+sizeof(stMe3630SendHead),pGGAJson,strlen(pGGAJson));
	me3630_SendData(DataBuf,SendLen,&Me3630Status);
	free(DataBuf);
#if DEBUG_JSON
//	int num;
//	while(SendLen--)
//	{
//		printf("%c",*(DataBuf+(num++)));
//	}
#endif
	s2j_delete_json_obj(JsonData);//在第二次使用json_student之前要把json结构体清除一次
	free(pGGAJson);
#if DEBUG_JSON
//	printf("%s",pGGAJson);
#endif
}

void  json_test_me3630(stMe3630SendHead *head)
{
	 
}

