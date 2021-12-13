#include <iostream>
#include<gl/glew.h>
#include<gl/freeglut.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <vector>
#include <random>
#include <math.h>
#include <fstream>
#include <string>
#include"readTriangleObj.h"


#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"
using namespace std;

//shader func
void makeVertexShader();
void makeFragmentShader();
void makeShaderID();
void InitBuffer();
void initTexture();

//call_back
void timer(int value);
void DrawSceneCall();
void Resize(int w, int h);
void keyboardCall(unsigned char key, int x, int y);


//func
void drawPlane();
void makeObj();
void drawtexture();


GLuint fragmentShader;
GLuint modelvertexShader;
GLuint ShaderID;

char* vertexSource;
char* fragmentSource;

int Wwidth = 800;
int Wheight = 600;


GLuint VAO[10];
GLuint VBO[30];

vector<glm::vec4> Vertex[10];
vector<glm::vec4> Nomal[10];
vector<glm::vec2> Texture[10];


unsigned int texture[4];
unsigned int pyramidtexture;

float angle = 0.0f;
float cameraRevolu = 0.0f;
float lightRevoluAngle = 0.0f;


float lightobjR = 30.0f; float lightL = 3.0f;

float cam_move_x = 0.0f; float cam_move_z = 12.0f; float cam_move_y = 0.0f;
float cameraR = 0.0f;


bool change = false;
bool Rotate = false;
bool checkjpg = false;


glm::vec3 objC = glm::vec3(0, 0, 0);
glm::vec3 lightPos = glm::vec3(0, 3.0f, 2.5f);
glm::vec3 lightPos2 = glm::vec3(0.0f);
glm::vec3 lightColor = glm::vec3(0.7f, 0.7f, 0.7f);
glm::vec3 lightColor2 = glm::vec3(1.0f);

glm::vec3 lightColorKind[4] = {
	glm::vec3(0.7f, 0.7f, 0.7f),
	glm::vec3(1,0,0),
	glm::vec3(0,1,0),
	glm::vec3(0,0,1)
};
int selectLightColor = 0;


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(Wwidth, Wheight);
	glutCreateWindow("texture example");

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
		cerr << "fail Initialize" << endl;
	else cout << "Initialize" << endl;

	makeShaderID();
	makeObj();
	InitBuffer();
	initTexture();
	glutDisplayFunc(DrawSceneCall);
	glutReshapeFunc(Resize);
	glutKeyboardFunc(keyboardCall);
	glutTimerFunc(1, timer, 1);
	glutMainLoop();
}

void timer(int value)
{
	if (Rotate == true)
	{
		angle += 3.0f;
	}

	glutPostRedisplay();
	glutTimerFunc(17, timer, value);
}

void DrawSceneCall()
{
	glClearColor(0.01f, 0.01f, 0.01f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	 
	glm::mat4 cameraRevoluMatrix = glm::mat4(1.0f);
	cameraRevoluMatrix = glm::translate(cameraRevoluMatrix, glm::vec3(0.0f, 5.0f, 1.0f));
	cameraRevoluMatrix = glm::rotate(cameraRevoluMatrix, glm::radians(cameraR), glm::vec3(0.0f, 1.0f, 0.0f));//공전
	glm::vec3 camerapos = glm::vec3(cameraRevoluMatrix * glm::vec4(cam_move_x, cam_move_y, cam_move_z, 1.0f));
	glm::vec3 cameradir = camerapos - glm::vec3(0, 0, 0);
	glm::vec3 worldup = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraRight = glm::normalize(glm::cross(worldup, glm::normalize(cameradir)));
	glm::vec3 cameraUp = glm::normalize(glm::cross(glm::normalize(cameradir), cameraRight));
	unsigned int cameraPosLocation = glGetUniformLocation(ShaderID, "viewPos");
	glUniform3fv(cameraPosLocation, 1, glm::value_ptr(camerapos));
	//뷰잉 변환
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(camerapos, -cameradir, cameraUp);
	//view = glm::rotate(view, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//view = glm::rotate(view, glm::radians(viewRR), glm::vec3(0.0f, 1.0f, 0.0f));
	unsigned int viewLocation = glGetUniformLocation(ShaderID, "viewTransform"); //--- 뷰잉 변환 설정
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), (float)800.0f / (float)800.0f, 0.5f, 50.0f);
	projection = glm::translate(projection, glm::vec3(0.0, 0.0, -5.0)); //--- 공간을 약간 뒤로 미뤄줌
	unsigned int projectionLocation = glGetUniformLocation(ShaderID, "projectionTransform"); //--- 투영 변환 값 설정
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));


	glBindVertexArray(VAO[0]);
	unsigned int TextureNoCheck = glGetUniformLocation(ShaderID, "Texturecheck");
	glUniform1i(TextureNoCheck, 2);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glUniform1i(glGetUniformLocation(ShaderID, "textureC"), 0);
	glm::mat4 Lightobj = glm::mat4(1.0f);
	//Lightobj = glm::scale(Lightobj, glm::vec3(1.0f, 1.0f, 1.0f));
	//Lightobj = glm::rotate(Lightobj, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
	Lightobj = glm::rotate(Lightobj, glm::radians(lightobjR), glm::vec3(0.0, 1.0, 0.0));
	Lightobj = glm::rotate(Lightobj, glm::radians(-30.0f), glm::vec3(1.0, 0.0, 0.0));
	Lightobj = glm::translate(Lightobj, glm::vec3(0.0f, 1.0f, lightL + 5.0f));
	//Lightobj = glm::rotate(Lightobj, glm::radians(30.0f), glm::vec3(0.0, 1.0, 0.0));
	Lightobj = glm::scale(Lightobj, glm::vec3(1.0f, 0.5f, 1.0f));
	unsigned int lightlocation = glGetUniformLocation(ShaderID, "modelTransform");
	glUniformMatrix4fv(lightlocation, 1, GL_FALSE, glm::value_ptr(Lightobj));
	glm::vec3 Llightobjcolor = glm::vec3(1.0, 0.0, 0.0);
	unsigned int Llightobj = glGetUniformLocation(ShaderID, "objColor");
	glUniform3fv(Llightobj, 1, glm::value_ptr(Llightobjcolor));

	glm::mat4 normaltrans = glm::mat4(1.0f);
	//normaltrans = glm::rotate(normaltrans, glm::radians(lightobjR), glm::vec3(0.0, 1.0, 0.0));
	unsigned int normaltransform = glGetUniformLocation(ShaderID, "normalTransform");
	glUniformMatrix4fv(normaltransform, 1, GL_FALSE, glm::value_ptr(normaltrans));
	glDrawArrays(GL_TRIANGLES, 0, Vertex[0].size());


	glm::mat4 LightPosition = glm::mat4(1.0f);
	//LightPosition = glm::rotate(LightPosition, glm::radians(lightobjR), glm::vec3(0.0f, 1.0f, 0.0f));
	//LightPosition = glm::rotate(LightPosition, glm::radians(-30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	LightPosition = glm::translate(LightPosition, glm::vec3(0.0f, 1.0f, lightL + 3.0f));
	unsigned int lightPosLocation = glGetUniformLocation(ShaderID, "LightPos"); // 조명 위치
	glUniform3fv(lightPosLocation, 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
	unsigned int lightColorLocation = glGetUniformLocation(ShaderID, "LightColor"); // 조명 색,세기
	glUniform3f(lightColorLocation, 1.0f, 1.0f, 1.0f);
	unsigned int LightTransformLocation = glGetUniformLocation(ShaderID, "LightTransform"); // 조명위치 변환(trans) 행렬
	glUniformMatrix4fv(LightTransformLocation, 1, GL_FALSE, glm::value_ptr(LightPosition));


	


	drawtexture();
	glutSwapBuffers();
}

void Resize(int w, int h)
{
	glViewport(0, 0, w, h);
	Wwidth = w;
	Wheight = h;
}

void keyboardCall(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'a':
		cam_move_x -= 0.2f; break;
	case 'r':
		cameraR += 5.0f; break;
	case'q':
		glutLeaveMainLoop();
		break;
		case 'p':
		change = !change;
		break;
	}
	glutPostRedisplay();
}


char* filetobuf(const string name)
{
	vector<char> tempFile;
	ifstream in(name, ios::binary);
	char temp;
	while (true) {
		noskipws(in);
		in >> temp;
		if (in.eof()) {
			tempFile.push_back(0);
			break;
		}
		else
			tempFile.push_back(temp);
	}
	char* addr = new char[tempFile.size()];
	for (int i = 0; i < tempFile.size(); i++) {
		addr[i] = tempFile[i];
	}
	return addr;
}

void makeVertexShader()
{
	vertexSource = filetobuf("VertexTextureShader.glsl");
	modelvertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(modelvertexShader, 1, &vertexSource, NULL);
	glCompileShader(modelvertexShader);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(modelvertexShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(modelvertexShader, 512, NULL, errorLog);
		cerr << "VERTEXSHADER ERROR: " << errorLog << endl;
	}
}

void makeFragmentShader()
{
	fragmentSource = filetobuf("FragmentTextureShader.glsl");
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		cerr << "FRAGMENT SHADER ERROR: " << errorLog << endl;
	}
}

void makeShaderID()
{
	makeVertexShader();
	makeFragmentShader();

	ShaderID = glCreateProgram();

	glAttachShader(ShaderID, modelvertexShader);
	glAttachShader(ShaderID, fragmentShader);

	glLinkProgram(ShaderID);
	GLint result;
	glGetProgramiv(ShaderID, GL_LINK_STATUS, &result);
	GLchar errorLog[512];
	if (!result) {
		glGetProgramInfoLog(ShaderID, 512, NULL, errorLog);
		cerr << "ShaderID0 Program ERROR: " << errorLog << endl;
	}

	glDeleteShader(modelvertexShader);
	glDeleteShader(fragmentShader);
	glUseProgram(ShaderID);
}


void InitBuffer()
{
	glGenVertexArrays(7, VAO);

	

	for (int i = 0; i < 7; i++)
	{

	glBindVertexArray(VAO[i]);
	glGenBuffers(3, &VBO[3*i]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[3 * i]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * Vertex[i].size(), &Vertex[i][0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[3 * i+1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * Nomal[i].size(), &Nomal[i][0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[3 * i+2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * Texture[i].size(), &Texture[i][0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
	glEnableVertexAttribArray(2);
	}
	//----------------------------------------------------
	
}

void makeObj()
{
	readTriangleObj("bottom.obj", Vertex[0], Texture[0], Nomal[0]);
	readTriangleObj("bottom.obj", Vertex[1], Texture[1], Nomal[1]);
	readTriangleObj("bottom.obj", Vertex[2], Texture[2], Nomal[2]);
	readTriangleObj("cart.obj", Vertex[3], Texture[3], Nomal[3]);
	readTriangleObj("handle.obj", Vertex[4], Texture[4], Nomal[4]);
	readTriangleObj("wheel1.obj", Vertex[5], Texture[5], Nomal[5]);
	readTriangleObj("wheel2.obj", Vertex[6], Texture[6], Nomal[6]);

}

void drawtexture()
{
	glUseProgram(ShaderID);


	glBindVertexArray(VAO[2]);
	unsigned int TextureCheck = glGetUniformLocation(ShaderID, "Texturecheck");
	glUniform1i(TextureCheck, 2);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glUniform1i(glGetUniformLocation(ShaderID, "textureC"), 0);
	glm::mat4 floor = glm::mat4(1.0f);
	floor = glm::rotate(floor, glm::radians(0.0f), glm::vec3(1, 0, 0));
	floor = glm::translate(floor, glm::vec3(0, 0, -5));
	floor = glm::scale(floor, glm::vec3(5.0, 5.0, 10.0));
	unsigned int floorLocation = glGetUniformLocation(ShaderID, "modelTransform");
	glUniformMatrix4fv(floorLocation, 1, GL_FALSE, glm::value_ptr(floor));
	glm::mat4 floorNormal = glm::mat4(1.0f);
	floorNormal = glm::rotate(floorNormal, glm::radians(0.0f), glm::vec3(0, 1, 0));
	unsigned int floorNormalLocation = glGetUniformLocation(ShaderID, "normalTransform");
	glUniformMatrix4fv(floorNormalLocation, 1, GL_FALSE, glm::value_ptr(floorNormal));
	glDrawArrays(GL_TRIANGLES, 0, Vertex[2].size());


	glBindVertexArray(VAO[1]);
	unsigned int TextureCheck1 = glGetUniformLocation(ShaderID, "Texturecheck");
	glUniform1i(TextureCheck1, 2);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glUniform1i(glGetUniformLocation(ShaderID, "textureC"), 0);
	glm::mat4 cubeTrasMatrix = glm::mat4(1.0f);
	cubeTrasMatrix = glm::rotate(cubeTrasMatrix, glm::radians(angle), glm::vec3(0, 1, 0));
	cubeTrasMatrix = glm::translate(cubeTrasMatrix, glm::vec3(0, 0, 2));
	cubeTrasMatrix = glm::scale(cubeTrasMatrix, glm::vec3(1.5, 2.5, 1.5));
	unsigned int cubeTransMatrixLocation = glGetUniformLocation(ShaderID, "modelTransform");
	glUniformMatrix4fv(cubeTransMatrixLocation, 1, GL_FALSE, glm::value_ptr(cubeTrasMatrix));
	glm::mat4 cubeNormalMatrix = glm::mat4(1.0f);
	cubeNormalMatrix = glm::rotate(cubeNormalMatrix, glm::radians(angle), glm::vec3(0, 1, 0));
	unsigned int cubeNormalMatrixLocation = glGetUniformLocation(ShaderID, "normalTransform");
	glUniformMatrix4fv(cubeNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(cubeNormalMatrix));
	glDrawArrays(GL_TRIANGLES, 0, Vertex[1].size());

	glBindVertexArray(VAO[3]);
	unsigned int TextureCheck3 = glGetUniformLocation(ShaderID, "Texturecheck");
	glUniform1i(TextureCheck3, 2);
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glActiveTexture(GL_TEXTURE0);
	glm::mat4 kart = glm::mat4(1.0f);
	kart = glm::translate(kart, glm::vec3(0.0, 0.0, 0.0));
	kart = glm::rotate(kart, glm::radians(angle), glm::vec3(0, 1, 0));
	kart = glm::scale(kart, glm::vec3(1.0, 1.0, 1.0));
	unsigned int kartTransMatrixLocation = glGetUniformLocation(ShaderID, "modelTransform");
	glUniformMatrix4fv(kartTransMatrixLocation, 1, GL_FALSE, glm::value_ptr(kart));
	glm::mat4 kartNormalMatrix = glm::mat4(1.0f);
	kartNormalMatrix = glm::rotate(kartNormalMatrix, glm::radians(angle), glm::vec3(0, 1, 0));
	unsigned int kartNormalMatrixLocation = glGetUniformLocation(ShaderID, "normalTransform");
	glUniformMatrix4fv(kartNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(kartNormalMatrix));
	glUniform1i(glGetUniformLocation(ShaderID, "textureC"), 0);
	glDrawArrays(GL_TRIANGLES, 0, Vertex[3].size());

	glBindVertexArray(VAO[4]);
	unsigned int TextureCheck4 = glGetUniformLocation(ShaderID, "Texturecheck");
	glUniform1i(TextureCheck4, 2);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glUniform1i(glGetUniformLocation(ShaderID, "textureC"), 0);
	glm::mat4 handle = glm::mat4(1.0f);
	handle = glm::rotate(handle, glm::radians(angle), glm::vec3(0, 1, 0));
	handle = glm::translate(handle, glm::vec3(0, 0, 0));
	handle = glm::scale(handle, glm::vec3(1.0, 1.0, 1.0));
	unsigned int handleTransMatrixLocation = glGetUniformLocation(ShaderID, "modelTransform");
	glUniformMatrix4fv(handleTransMatrixLocation, 1, GL_FALSE, glm::value_ptr(handle));
	glm::mat4 handleNormalMatrix = glm::mat4(1.0f);
	handleNormalMatrix = glm::rotate(handleNormalMatrix, glm::radians(angle), glm::vec3(0, 1, 0));
	unsigned int handleNormalMatrixLocation = glGetUniformLocation(ShaderID, "normalTransform");
	glUniformMatrix4fv(handleNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(handleNormalMatrix));
	glDrawArrays(GL_TRIANGLES, 0, Vertex[4].size());

	glBindVertexArray(VAO[5]);
	unsigned int TextureCheck5 = glGetUniformLocation(ShaderID, "Texturecheck");
	glUniform1i(TextureCheck5, 2);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glUniform1i(glGetUniformLocation(ShaderID, "textureC"), 0);
	glm::mat4 wheelfront = glm::mat4(1.0f);
	//wheelfront = glm::rotate(wheelfront, glm::radians(angle), glm::vec3(0, 1, 0));
	wheelfront = glm::translate(wheelfront, glm::vec3(1.8, 0, 1.0));
	wheelfront = glm::scale(wheelfront, glm::vec3(1.0, 1.0, 1.0));
	unsigned int wheelfrontTransMatrixLocation = glGetUniformLocation(ShaderID, "modelTransform");
	glUniformMatrix4fv(wheelfrontTransMatrixLocation, 1, GL_FALSE, glm::value_ptr(wheelfront));
	glm::mat4 wheelfrontNormalMatrix = glm::mat4(1.0f);
	wheelfrontNormalMatrix = glm::rotate(wheelfrontNormalMatrix, glm::radians(angle), glm::vec3(0, 1, 0));
	unsigned int wheelfrontNormalMatrixLocation = glGetUniformLocation(ShaderID, "normalTransform");
	glUniformMatrix4fv(wheelfrontNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(wheelfrontNormalMatrix));
	glDrawArrays(GL_TRIANGLES, 0, Vertex[5].size());

	glBindVertexArray(VAO[6]);
	unsigned int TextureCheck6 = glGetUniformLocation(ShaderID, "Texturecheck");
	glUniform1i(TextureCheck6, 2);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glUniform1i(glGetUniformLocation(ShaderID, "textureC"), 0);
	glm::mat4 wheelback = glm::mat4(1.0f);
	//wheelback = glm::rotate(wheelback, glm::radians(angle), glm::vec3(0, 1, 0));
	wheelback = glm::translate(wheelback, glm::vec3(1.8, 0, -1.0));
	wheelback = glm::scale(wheelback, glm::vec3(1.0, 1.0, 1.0));
	unsigned int wheelbackTransMatrixLocation = glGetUniformLocation(ShaderID, "modelTransform");
	glUniformMatrix4fv(wheelbackTransMatrixLocation, 1, GL_FALSE, glm::value_ptr(wheelback));
	glm::mat4 wheelbackNormalMatrix = glm::mat4(1.0f);
	wheelbackNormalMatrix = glm::rotate(wheelbackNormalMatrix, glm::radians(angle), glm::vec3(0, 1, 0));
	unsigned int wheelbackNormalMatrixLocation = glGetUniformLocation(ShaderID, "normalTransform");
	glUniformMatrix4fv(wheelbackNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(wheelbackNormalMatrix));
	glDrawArrays(GL_TRIANGLES, 0, Vertex[6].size());
	



}

void initTexture()
{

	glGenTextures(2, &texture[0]);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int widthImage, heightImage, numberOfChannel;
	unsigned char* data = stbi_load("Cubepicture.bmp", &widthImage, &heightImage, &numberOfChannel, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthImage, heightImage, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int pwidthImage, pheightImage, pnumberOfChannel;
	unsigned char* pdata = stbi_load("pyramidpicture.bmp", &pwidthImage, &pheightImage, &pnumberOfChannel, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pwidthImage, pheightImage, 0, GL_RGB, GL_UNSIGNED_BYTE, pdata);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(pdata);

	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int BackwidthImage, BackheightImage, BacknumberOfChannel;
	unsigned char* Backdata = stbi_load("sky_texture.jpg", &BackwidthImage, &BackheightImage, &BacknumberOfChannel, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BackwidthImage, BackheightImage, 0, GL_RGB, GL_UNSIGNED_BYTE, Backdata);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(Backdata);
}
