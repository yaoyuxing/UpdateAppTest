

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
cJSON *json_student = struct_to_json(&orignal_student_obj);//�ṹ��ת��json�ṹ��
char *json_text=cJSON_Print(json_student);//��json�ṹ��ת���ַ���
cJSON *char_json=cJSON_Parse(cJSON_Print(json_student)); //json�ַ���תjson�ṹ��
Student * converted_student_obj = json_to_struct(json_student);//json�ṹ��ת���ؽṹ��

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
    json_student = struct_to_json(&orignal_student_obj);//�ṹ��ת��json�ṹ��
	
	p=cJSON_PrintUnformatted(json_student);//���ص���JSON�ṹ���ַ���
	
	s2j_delete_json_obj(json_student);//�ڵڶ���ʹ��json_student֮ǰҪ��json�ṹ�����һ��
	printf("%s",p); //��json�ṹ��ת�����ַ���
	json_student=cJSON_Parse(p);//���ַ���ת����json�ṹ��
	

	
    /* deserialize Student structure object */
    Student * converted_student_obj = json_to_struct(json_student);//json�ṹ��ת�ṹ��

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
�����͸�����Ľṹ��ת����JSON�ṹ��
��ת����������������ͷ
	int ApiLogo;			//api��ʶ�룬�ɷ�����������ռ��
	int	RequestID;			//����ID���ɿͻ��˸������������ڽ��յ����ݺ�᷵���ѽ������������Ӧ
	int DataLen;			//���ݳ���
	int	DataOutWay;			//�������л���ʽ 1 msgpack ���л�
	int PackageInsideNum;	//�����ڷְ��ڵ����У���0��ʼ
	int	PackageNum;			//���ݵķְ���������СΪ1
*/

/*
GGA�����JSON����
GgaData:�����GGA���ַ�����
rturn_data�����������ö��JSON������
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
	stMe3630SendHead DataHead;//���ݵ�ͷ
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
	
	cJSON_AddStringToObject(JsonData,"GGA",GgaData);//��GGA���ַ���ת����JSON�ṹ������
	pGGAJson =cJSON_PrintUnformatted(JsonData);//���ص���JSON�ṹ���ַ���
	
	
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
	s2j_delete_json_obj(JsonData);//�ڵڶ���ʹ��json_student֮ǰҪ��json�ṹ�����һ��
	free(pGGAJson);
#if DEBUG_JSON
//	printf("%s",pGGAJson);
#endif
}

void  json_test_me3630(stMe3630SendHead *head)
{
	 
}

