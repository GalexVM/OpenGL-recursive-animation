#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>//<>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef STBI_INCLUDE_STB_IMAGE_H
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <filesystem>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <unordered_map>
#include <cmath>

#include "shader.h"

using namespace std;
namespace fs = std::filesystem;


//de este punto van a salir las coordenadas de los puntos x y z que le pasaremos al shader
//tambien va a tener los colores de cad cara con c1 c2 y c3
struct Punto{
	float x, y, z, c1, c2, c3, t1, t2;
	Punto(float _x, float _y, float _z, glm::vec3 color, float _t1, float _t2){
		x = _x;
		y = _y;
		z = _z;
		c1 = color.x;
		c2 = color.y;
		c3 = color.z;
		t1 = _t1;
		t2 = _t2;
	}
};


class Cubo{
	public:
		vector<Punto> vertices_centros; //almacenamos los vertices que van a salir de este centro
		glm::vec3 dimension; //que tan grande vana  ser los cubos
		unsigned int VAO, VBO;
		glm::vec3 centro; //para dibujar
		int flag=1;

		unsigned int mytexture;
		unsigned char *data;

		
		Cubo(glm::vec3 _dimension, glm::vec3 ubicacion,vector<glm::vec3> color){
			dimension = _dimension;
			centro = ubicacion;
			color[0].x=1.0f;
			color[0].y=0.0f;
			color[0].z=0.0f;
			
			//Caras
			//--------------------------------------------------------------------------------
			//vertices de en frente
			vertices_centros.push_back(Punto(-dimension.x, -dimension.y, dimension.z, color[0], 0.0f, 0.0f));//BL
			vertices_centros.push_back(Punto(dimension.x, -dimension.y, dimension.z,color[0], 1.0f, 0.0f));//BR
			vertices_centros.push_back(Punto(dimension.x, dimension.y, dimension.z,color[0], 1.0f, 1.0f));//TR
			vertices_centros.push_back(Punto(dimension.x, dimension.y, dimension.z,color[0], 1.0f, 1.0f));//TR
			vertices_centros.push_back(Punto(-dimension.x, dimension.y, dimension.z,color[0], 0.0f, 1.0f));//TL
			vertices_centros.push_back(Punto(-dimension.x, -dimension.y, dimension.z,color[0], 0.0f, 0.0f));//BL
			
			//--------------------------------------------------------------------------------
			//vertices de arriba
			vertices_centros.push_back(Punto(-dimension.x, dimension.y, -dimension.z,color[1], 0.0f, 0.0f));
			vertices_centros.push_back(Punto(dimension.x, dimension.y, -dimension.z,color[1], 1.0f, 0.0f));
			vertices_centros.push_back(Punto(dimension.x, dimension.y, dimension.z,color[1], 1.0f, 1.0f));
			vertices_centros.push_back(Punto(dimension.x, dimension.y, dimension.z,color[1], 1.0f, 1.0f));
			vertices_centros.push_back(Punto(-dimension.x, dimension.y, dimension.z,color[1], 0.0f, 1.0f));
			vertices_centros.push_back(Punto(-dimension.x, dimension.y, -dimension.z,color[1], 0.0f, 0.0f));
			
			//---------------------------------------------------------------------------------
			//vertices de la izquierda
			vertices_centros.push_back(Punto(-dimension.x, dimension.y, dimension.z,color[2], 0.0f, 0.0f));
			vertices_centros.push_back(Punto(-dimension.x, dimension.y, -dimension.z,color[2], 1.0f, 0.0f));
			vertices_centros.push_back(Punto(-dimension.x, -dimension.y, -dimension.z,color[2], 1.0f, 1.0f));
			vertices_centros.push_back(Punto(-dimension.x, -dimension.y, -dimension.z,color[2], 1.0f, 1.0f));
			vertices_centros.push_back(Punto(-dimension.x, -dimension.y, dimension.z,color[2], 0.0f, 1.0f));
			vertices_centros.push_back(Punto(-dimension.x, dimension.y, dimension.z,color[2], 0.0f, 0.0f));
			
			//---------------------------------------------------------------------------------
			//vertices de abajo
			vertices_centros.push_back(Punto(-dimension.x, -dimension.y, -dimension.z,color[3], 0.0f, 0.0f));
			vertices_centros.push_back(Punto(dimension.x, -dimension.y, -dimension.z,color[3], 1.0f, 0.0f));
			vertices_centros.push_back(Punto(dimension.x, -dimension.y, dimension.z,color[3], 1.0f, 1.0f));
			vertices_centros.push_back(Punto(dimension.x, -dimension.y, dimension.z,color[3], 1.0f, 1.0f));
			vertices_centros.push_back(Punto(-dimension.x, -dimension.y, dimension.z,color[3], 0.0f, 1.0f));
			vertices_centros.push_back(Punto(-dimension.x, -dimension.y, -dimension.z,color[3], 0.0f, 0.0f));
			
			//---------------------------------------------------------------------------------
			//vertices de la derecha
			vertices_centros.push_back(Punto(dimension.x, dimension.y, dimension.z,color[4], 0.0f, 0.0f));
			vertices_centros.push_back(Punto(dimension.x, dimension.y, -dimension.z,color[4], 1.0f, 0.0f));
			vertices_centros.push_back(Punto(dimension.x, -dimension.y, -dimension.z,color[4], 1.0f, 1.0f));
			vertices_centros.push_back(Punto(dimension.x, -dimension.y, -dimension.z,color[4], 1.0f, 1.0f));
			vertices_centros.push_back(Punto(dimension.x, -dimension.y, dimension.z,color[4], 0.0f, 1.0f));
			vertices_centros.push_back(Punto(dimension.x, dimension.y, dimension.z,color[4], 0.0f, 0.0f));
			
			//--------------------------------------------------------------------------------
			//vertices traseros
			vertices_centros.push_back(Punto(-dimension.x, -dimension.y, -dimension.z,color[5], 0.0f, 0.0f));
			vertices_centros.push_back(Punto(dimension.x, -dimension.y, -dimension.z,color[5], 1.0f, 0.0f));
			vertices_centros.push_back(Punto(dimension.x, dimension.y, -dimension.z,color[5], 1.0f, 1.0f));
			vertices_centros.push_back(Punto(dimension.x, dimension.y, -dimension.z,color[5], 1.0f, 1.0f));
			vertices_centros.push_back(Punto(-dimension.x, dimension.y, -dimension.z,color[5], 0.0f, 1.0f));
			vertices_centros.push_back(Punto(-dimension.x, -dimension.y, -dimension.z,color[5], 0.0f, 0.0f));
		
		}			
		
		//Funciones para manejar los buffers
		void generarBuffers() {
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);

			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, vertices_centros.size() * 6 * sizeof(float), &vertices_centros[0], GL_STATIC_DRAW);
			//vertex position
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			// color attribute
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
			//texture attribute
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    		glEnableVertexAttribArray(2);
		
		}
		void updateBuffers() { 
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, vertices_centros.size() * 8 * sizeof(float), &vertices_centros[0], GL_STATIC_DRAW);
		}
		void deleteBuffers() {
			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);
		}
		void print()
		{
			if(flag==1)
			{
				for(int x=0;x<36;x++)
				{
					cout<<"-----------------------------"<<endl;
					cout<<"x: "<<vertices_centros[x].x<<endl;
					cout<<"y: "<<vertices_centros[x].y<<endl;
					cout<<"z: "<<vertices_centros[x].z<<endl;
				}
				flag=0;
			}
		}
		void cargar_texturas(Shader* ourShader, string tex){
			glGenTextures(1, &mytexture);
			glBindTexture(GL_TEXTURE_2D, mytexture); 
			//wrap parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
   			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			//filter parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			//cout<<"Parametros de texturas\n";
			//load image and create mipmaps
			int width, height, nrChannels;
			fs::path p = fs::current_path();
			fs::path p_current = p.parent_path();
			std::stringstream ss;
			ss << std::quoted( p_current.string() );
			std::string out;
			ss >> std::quoted(out);
			std::string img_path = out + tex; 
			//cout<<"string\n";
			//crear objeto
			cout<<"Path: "<<img_path<<endl;
			data = stbi_load(
				img_path.c_str(),
				&width, &height, &nrChannels, 0);
			//cout<<"load: "<<"w: "<<width<<" h: "<<height<<" c: "<<nrChannels<<endl;
			if(data){
				//cout<<"data not null\n";
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				//cout<<"generar textura\n";
        		glGenerateMipmap(GL_TEXTURE_2D);
				//cout<<"generar mipmaps\n";
			}else{
				//cout << "Failed to load texture" << std::endl;
			}
			//liberar imagen
			stbi_image_free(data);
			//cargar uniform
			ourShader->use();
			glUniform1i(glGetUniformLocation(ourShader->ID, "mytexture"), 0);
			//cout<<"Uniforms\n";
		}
		void activar_texturas(){
			glActiveTexture(GL_TEXTURE0);
       		glBindTexture(GL_TEXTURE_2D, mytexture);
			//cout<<"Textura activa\n";
		}
		void dibujar_cubito(Shader* ourShader) { 
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, centro);
			ourShader->setMat4("model", model);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			//cout<<"Cubito dibujado\n";
		}
		//Mover los cubos
		void move(glm::vec3 pivot, Shader* ourShader, float degrees) {
			glm::mat4 model = glm::mat4(1.0f);
			//aplicamos la matriz de modelo a nuestro shader para que todos los centros se muevan
			glm::mat4 rot = glm::mat4(1.0f);
			rot = glm::rotate(rot, glm::radians(degrees), pivot);
			glm::vec4 r = rot * glm::vec4(centro, 1.0f);
			centro = r;
			model = glm::translate(model, centro);
			//actualizamos tambien os vertices de cada cubito en particular para tener control sobre ellos
			glm::vec4 nuevos_vertices;
			glm::mat4 rotMatrix = glm::mat4(1.0f);
			for (int i = 0; i < vertices_centros.size(); i++)
			{
				nuevos_vertices = glm::vec4(vertices_centros[i].x, vertices_centros[i].y, vertices_centros[i].z, 1.0f);
				nuevos_vertices = glm::rotate(rotMatrix, glm::radians(degrees), pivot) * nuevos_vertices;
				vertices_centros[i].x = nuevos_vertices.x;
				vertices_centros[i].y = nuevos_vertices.y;
				vertices_centros[i].z = nuevos_vertices.z;
			}
			updateBuffers();
			ourShader->setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		void translate(glm::vec3 pivot, Shader* ourShader){
			glm::mat4 model = glm::mat4(1.0f);
			//aplicamos la matriz de modelo a nuestro shader para que todos los centros se muevan
			glm::mat4 tra = glm::mat4(1.0f);
			tra = glm::translate(tra, pivot);
			
			glm::vec4 r = tra * glm::vec4(centro, 1.0f);
			centro = r;
			model = glm::translate(model, centro);
			//actualizamos tambien os vertices de cada cubito en particular para tener control sobre ellos
			glm::vec4 nuevos_vertices;
			glm::mat4 trasMatrix = glm::mat4(1.0f);
			for (int i = 0; i < vertices_centros.size(); i++)
			{
				nuevos_vertices = glm::vec4(vertices_centros[i].x, vertices_centros[i].y, vertices_centros[i].z, 1.0f);
				nuevos_vertices = glm::translate(trasMatrix, pivot) * nuevos_vertices;
				vertices_centros[i].x = nuevos_vertices.x;
				vertices_centros[i].y = nuevos_vertices.y;
				vertices_centros[i].z = nuevos_vertices.z;
			}
			updateBuffers();
			ourShader->setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}


};

//-------------------------------------
class Dona{
	protected:
		vector<Cubo> cubitos;
		GLFWwindow* window;
		Shader* shader;
		glm::vec3 origin;
		glm::vec3* c;
		
	public:

		vector<string> cambios;
		vector<string> solution;
		vector<glm::vec3> cubePositions;
		float degree = 1.0f;
		Dona(GLFWwindow* _window,Shader* _shader,float dimension, int pos, glm::vec3 org) //pos: 0=z, 1=x, 2=y
		{
			
			//parte derecha del cubito (positivo)
			float dimension_derecha = dimension * 2;//+(dimension/6);
			//parte izquierda del cubito (negativo)
			float dimension_izquierda = -dimension_derecha;
			window = _window;
			shader = _shader;
			origin = org;

			/*cout << "dimension: "<<dimension<<endl;
			cout <<"pos: "<<pos;
			cout<<"centro: "<<org.x<<", "<<org.y<<", "<<org.z<<", "<<endl;*/

			if(pos == 0){//mirando z adelante
				cubePositions = {
					glm::vec3(origin.x +dimension_izquierda,  origin.y +dimension_derecha, origin.z +dimension_derecha),				//1
					glm::vec3(origin.x +0.0f, origin.y +dimension_derecha, origin.z +dimension_derecha),								//2
					glm::vec3(origin.x +dimension_derecha,  origin.y +dimension_derecha, origin.z +dimension_derecha),					//3
					glm::vec3(origin.x +dimension_izquierda,  origin.y +0.0f,origin.z +dimension_derecha),								//4
					//glm::vec3origin.x +(0.0f,  origin.y +0.0f, origin.z +dimension_derecha),											
					glm::vec3(origin.x +dimension_derecha, origin.y +0.0f,origin.z +dimension_derecha),									//5
					glm::vec3(origin.x +dimension_izquierda, origin.y +dimension_izquierda, origin.z +dimension_derecha),				//6
					glm::vec3(origin.x +0.0f, origin.y +dimension_izquierda,origin.z + dimension_derecha),								//7
					glm::vec3(origin.x +dimension_derecha, origin.y +dimension_izquierda,origin.z + dimension_derecha),					//8

					glm::vec3(origin.x +dimension_izquierda, origin.y +dimension_derecha, origin.z +0.0f),								//9
					glm::vec3(origin.x +0.0f, origin.y +dimension_derecha,origin.z + 0.0f),												//10
					glm::vec3(origin.x +dimension_derecha, origin.y +dimension_derecha,origin.z + 0.0f),								//11
					glm::vec3(origin.x +dimension_izquierda, origin.y +0.0f,origin.z + 0.0f),											//12
					glm::vec3(origin.x +0.0f, origin.y + 0.0f,origin.z + 0.0f),															//13
					glm::vec3(origin.x +dimension_derecha, origin.y +0.0f,origin.z + 0.0f),												//14
					glm::vec3(origin.x +dimension_izquierda, origin.y + dimension_izquierda, origin.z +0.0f),							//15
					glm::vec3(origin.x +0.0f, origin.y +dimension_izquierda, origin.z +0.0f),											//16
					glm::vec3(origin.x +dimension_derecha,  origin.y +dimension_izquierda, origin.z +0.0f),								//17

					glm::vec3(origin.x +dimension_izquierda, origin.y +dimension_derecha, origin.z +dimension_izquierda),				//18
					glm::vec3(origin.x +0.0f, origin.y +dimension_derecha, origin.z +dimension_izquierda),								//19
					glm::vec3(origin.x +dimension_derecha, origin.y +dimension_derecha, origin.z +dimension_izquierda),					//20
					glm::vec3(origin.x +dimension_izquierda, origin.y +0.0f,origin.z +dimension_izquierda),								//21
					glm::vec3(origin.x +0.0f, origin.y +0.0f,origin.z +dimension_izquierda),											//22
					glm::vec3(origin.x +dimension_derecha, origin.y + 0.0f,origin.z +dimension_izquierda),								//23
					glm::vec3(origin.x +dimension_izquierda, origin.y + dimension_izquierda, origin.z +dimension_izquierda),			//24
					glm::vec3(origin.x +0.0f, origin.y +dimension_izquierda, origin.z +dimension_izquierda),							//25
					glm::vec3(origin.x +dimension_derecha, origin.y + dimension_izquierda, origin.z +dimension_izquierda)				//26							//7
				};
			}else if(pos == 2){//mirnado z atrás
				cubePositions = {
					glm::vec3(origin.x +dimension_izquierda,  origin.y +dimension_derecha, origin.z +dimension_derecha),				//1
					glm::vec3(origin.x +0.0f, origin.y +dimension_derecha, origin.z +dimension_derecha),								//2
					glm::vec3(origin.x +dimension_derecha,  origin.y +dimension_derecha, origin.z +dimension_derecha),					//3
					glm::vec3(origin.x +dimension_izquierda,  origin.y +0.0f,origin.z +dimension_derecha),								//4
					glm::vec3(origin.x +0.0f,  origin.y +0.0f, origin.z +dimension_derecha),											//5
					glm::vec3(origin.x +dimension_derecha, origin.y +0.0f,origin.z +dimension_derecha),									//6
					glm::vec3(origin.x +dimension_izquierda, origin.y +dimension_izquierda, origin.z +dimension_derecha),				//7
					glm::vec3(origin.x +0.0f, origin.y +dimension_izquierda,origin.z + dimension_derecha),								//8
					glm::vec3(origin.x +dimension_derecha, origin.y +dimension_izquierda,origin.z + dimension_derecha),					//9

					glm::vec3(origin.x +dimension_izquierda, origin.y +dimension_derecha, origin.z +0.0f),								//10
					glm::vec3(origin.x +0.0f, origin.y +dimension_derecha,origin.z + 0.0f),												//11
					glm::vec3(origin.x +dimension_derecha, origin.y +dimension_derecha,origin.z + 0.0f),								//12
					glm::vec3(origin.x +dimension_izquierda, origin.y +0.0f,origin.z + 0.0f),											//13
					glm::vec3(origin.x +0.0f, origin.y + 0.0f,origin.z + 0.0f),															//14
					glm::vec3(origin.x +dimension_derecha, origin.y +0.0f,origin.z + 0.0f),												//15
					glm::vec3(origin.x +dimension_izquierda, origin.y + dimension_izquierda, origin.z +0.0f),							//16
					glm::vec3(origin.x +0.0f, origin.y +dimension_izquierda, origin.z +0.0f),											//17
					glm::vec3(origin.x +dimension_derecha,  origin.y +dimension_izquierda, origin.z +0.0f),								//18

					glm::vec3(origin.x +dimension_izquierda, origin.y +dimension_derecha, origin.z +dimension_izquierda),				//19
					glm::vec3(origin.x +0.0f, origin.y +dimension_derecha, origin.z +dimension_izquierda),								//20
					glm::vec3(origin.x +dimension_derecha, origin.y +dimension_derecha, origin.z +dimension_izquierda),					//21
					glm::vec3(origin.x +dimension_izquierda, origin.y +0.0f,origin.z +dimension_izquierda),								//22
					//glm::vec3(origin.x +0.0f, origin.y +0.0f,origin.z +dimension_izquierda),												
					glm::vec3(origin.x +dimension_derecha, origin.y + 0.0f,origin.z +dimension_izquierda),								//23
					glm::vec3(origin.x +dimension_izquierda, origin.y + dimension_izquierda, origin.z +dimension_izquierda),			//24
					glm::vec3(origin.x +0.0f, origin.y +dimension_izquierda, origin.z +dimension_izquierda),							//25
					glm::vec3(origin.x +dimension_derecha, origin.y + dimension_izquierda, origin.z +dimension_izquierda)				//26
				};

			}else if(pos == 1){//mirando Y arriba
				cubePositions = {
					glm::vec3(origin.x +dimension_izquierda,  origin.y +dimension_derecha, origin.z +dimension_derecha),					//1
					glm::vec3(origin.x +0.0f, origin.y +dimension_derecha, origin.z +dimension_derecha),									//2
					glm::vec3(origin.x +dimension_derecha,  origin.y +dimension_derecha, origin.z +dimension_derecha),					//3
					glm::vec3(origin.x +dimension_izquierda,  origin.y +0.0f,origin.z +dimension_derecha),								//4
					glm::vec3(origin.x +0.0f,  origin.y +0.0f, origin.z +dimension_derecha),											//5
					glm::vec3(origin.x +dimension_derecha, origin.y +0.0f,origin.z +dimension_derecha),									//6
					glm::vec3(origin.x +dimension_izquierda, origin.y +dimension_izquierda, origin.z +dimension_derecha),				//7
					glm::vec3(origin.x +0.0f, origin.y +dimension_izquierda,origin.z + dimension_derecha),								//8!!
					glm::vec3(origin.x +dimension_derecha, origin.y +dimension_izquierda,origin.z + dimension_derecha),					//9

					glm::vec3(origin.x +dimension_izquierda, origin.y +dimension_derecha, origin.z +0.0f),								//10
					//glm::vec3(origin.x +0.0f, origin.y +dimension_derecha,origin.z + 0.0f),												//11
					glm::vec3(origin.x +dimension_derecha, origin.y +dimension_derecha,origin.z + 0.0f),									//12
					glm::vec3(origin.x +dimension_izquierda, origin.y +0.0f,origin.z + 0.0f),											//13
					glm::vec3(origin.x +0.0f, origin.y + 0.0f,origin.z + 0.0f),
					glm::vec3(origin.x +dimension_derecha, origin.y +0.0f,origin.z + 0.0f),												//14
					glm::vec3(origin.x +dimension_izquierda, origin.y + dimension_izquierda, origin.z +0.0f),								//15
					glm::vec3(origin.x +0.0f, origin.y +dimension_izquierda, origin.z +0.0f),												//16
					glm::vec3(origin.x +dimension_derecha,  origin.y +dimension_izquierda, origin.z +0.0f),								//17

					glm::vec3(origin.x +dimension_izquierda, origin.y +dimension_derecha, origin.z +dimension_izquierda),				//18
					glm::vec3(origin.x +0.0f, origin.y +dimension_derecha, origin.z +dimension_izquierda),								//19
					glm::vec3(origin.x +dimension_derecha, origin.y +dimension_derecha, origin.z +dimension_izquierda),					//20
					glm::vec3(origin.x +dimension_izquierda, origin.y +0.0f,origin.z +dimension_izquierda),								//21
					glm::vec3(origin.x +0.0f, origin.y +0.0f,origin.z +dimension_izquierda),												//22
					glm::vec3(origin.x +dimension_derecha, origin.y + 0.0f,origin.z +dimension_izquierda),								//23
					glm::vec3(origin.x +dimension_izquierda, origin.y + dimension_izquierda, origin.z +dimension_izquierda),				//24
					glm::vec3(origin.x +0.0f, origin.y +dimension_izquierda, origin.z +dimension_izquierda),								//25
					glm::vec3(origin.x +dimension_derecha, origin.y + dimension_izquierda, origin.z +dimension_izquierda)				
											
				};
			}else if(pos == 3){//mirando Y abajo
				cubePositions = {
					glm::vec3(origin.x +dimension_izquierda,  origin.y +dimension_derecha, origin.z +dimension_derecha),					//1
					glm::vec3(origin.x +0.0f, origin.y +dimension_derecha, origin.z +dimension_derecha),									//2
					glm::vec3(origin.x +dimension_derecha,  origin.y +dimension_derecha, origin.z +dimension_derecha),					//3
					glm::vec3(origin.x +dimension_izquierda,  origin.y +0.0f,origin.z +dimension_derecha),								//4
					glm::vec3(origin.x +0.0f,  origin.y +0.0f, origin.z +dimension_derecha),											//5
					glm::vec3(origin.x +dimension_derecha, origin.y +0.0f,origin.z +dimension_derecha),									//6
					glm::vec3(origin.x +dimension_izquierda, origin.y +dimension_izquierda, origin.z +dimension_derecha),				//7
					glm::vec3(origin.x +0.0f, origin.y +dimension_izquierda,origin.z + dimension_derecha),								//8!!
					glm::vec3(origin.x +dimension_derecha, origin.y +dimension_izquierda,origin.z + dimension_derecha),					//9

					glm::vec3(origin.x +dimension_izquierda, origin.y +dimension_derecha, origin.z +0.0f),								//10
					glm::vec3(origin.x +0.0f, origin.y +dimension_derecha,origin.z + 0.0f),												//11
					glm::vec3(origin.x +dimension_derecha, origin.y +dimension_derecha,origin.z + 0.0f),									//12
					glm::vec3(origin.x +dimension_izquierda, origin.y +0.0f,origin.z + 0.0f),											//13
					glm::vec3(origin.x +0.0f, origin.y + 0.0f,origin.z + 0.0f),
					glm::vec3(origin.x +dimension_derecha, origin.y +0.0f,origin.z + 0.0f),												//14
					glm::vec3(origin.x +dimension_izquierda, origin.y + dimension_izquierda, origin.z +0.0f),								//15
					//glm::vec3(origin.x +0.0f, origin.y +dimension_izquierda, origin.z +0.0f),												//16
					glm::vec3(origin.x +dimension_derecha,  origin.y +dimension_izquierda, origin.z +0.0f),								//17

					glm::vec3(origin.x +dimension_izquierda, origin.y +dimension_derecha, origin.z +dimension_izquierda),				//18
					glm::vec3(origin.x +0.0f, origin.y +dimension_derecha, origin.z +dimension_izquierda),								//19
					glm::vec3(origin.x +dimension_derecha, origin.y +dimension_derecha, origin.z +dimension_izquierda),					//20
					glm::vec3(origin.x +dimension_izquierda, origin.y +0.0f,origin.z +dimension_izquierda),								//21
					glm::vec3(origin.x +0.0f, origin.y +0.0f,origin.z +dimension_izquierda),												//22
					glm::vec3(origin.x +dimension_derecha, origin.y + 0.0f,origin.z +dimension_izquierda),								//23
					glm::vec3(origin.x +dimension_izquierda, origin.y + dimension_izquierda, origin.z +dimension_izquierda),				//24
					glm::vec3(origin.x +0.0f, origin.y +dimension_izquierda, origin.z +dimension_izquierda),								//25
					glm::vec3(origin.x +dimension_derecha, origin.y + dimension_izquierda, origin.z +dimension_izquierda)				
											
				};
			}

			
			//definimos los colores que usaremos
			glm::vec3 colors[] = {
				glm::vec3(0.0f,0.0f,0.0f),			//Negro
				glm::vec3(1.0f,1.0f,1.0f),			//Blanco
				glm::vec3(1.0f,0.5f,0.0f),			//Naranja  
				glm::vec3(0.8f,0.15f,1.0f),			//Morado  
				glm::vec3(1.0f,0.025f,0.25f),		//Rojo
				glm::vec3(0.224f,1.0f,0.078f),		//Verde 
				glm::vec3(0.15f,0.35f,1.0f)			//Azul
			};
			
			//definimos que colores va a tener cada cara segun su ubicacion
			int color_correspondiente[][6] = {
			   //F T L D R B
				{4,1,5,3,4,3},
				{4,1,2,4,3,2},
				{4,1,3,2,1,1},
				{4,1,2,3,2,5},
				{4,3,4,1,2,2},
				{4,2,5,3,5,1},
				{5,4,1,1,3,1},
				{4,5,2,5,2,1},
				{4,2,5,3,4,4},
				{3,3,5,1,3,2},
				{2,1,3,2,3,1},
				{1,2,5,3,1,3},
				{1,5,2,4,3,4},
				{3,4,3,5,1,2},
				{2,1,5,2,2,1},
				{3,3,2,1,4,2},
				{2,2,3,2,4,4},
				{1,2,1,3,5,5},
				{5,4,1,2,1,1},
				{1,1,2,1,5,3},
				{1,3,2,3,3,4},
				{5,5,5,2,1,1},
				{1,1,2,3,4,2},
				{5,2,3,1,1,4},
				{2,4,3,1,3,3},
				{1,3,4,3,6,2}	
			};
			
			vector<glm::vec3> color_cubito;
			
			for(int i=0;i<26;i++)
			{
				//rellenamos el color de cada cara de un cubitp
				for (int j = 0; j < 6; j++) 
				{
					color_cubito.push_back(colors[color_correspondiente[i][j]]);
				}	
				//lo almacenamos en un vector para luego cuando se dibuje tener guardado la posicion y el color
				//if(pos == 0)
					cubitos.push_back(Cubo(glm::vec3(dimension,dimension,dimension), cubePositions[i],color_cubito));
				//else
					//cubitos.push_back(Cubo(glm::vec3(dimension,dimension*3,dimension), cubePositions[i],color_cubito));
				
				color_cubito.clear();
			}

			generarBuffers();
		}
		
	//Para manejar los buffers
	virtual void generarBuffers() {
		for (int i = 0; i < cubitos.size(); i++){
			cubitos[i].generarBuffers();
			cubitos[i].cargar_texturas(shader, "\\OwnProjects\\Final_1\\textures\\tierra2.jpg");
		}
	}
	
	void deleteBuffers() {
		for (int i = 0; i < cubitos.size(); i++)
		{
			cubitos[i].deleteBuffers();
		}
	}
	//----------------
	virtual void dibujar_rubik() {	
		//cout<<"dona normal dibujada\n";
		for (int i = 0; i < cubitos.size(); i++)
		{
			cubitos[i].activar_texturas();
			cubitos[i].dibujar_cubito(shader);
		}
			
	}
	
	//Saber los movimientos
	void guardarMovimiento(char sideMove) {
		if (degree == -1) {
			cambios.push_back(string(1, sideMove));
			cout << sideMove << " ";
		}
		else {
			string s(1, sideMove);
			s.push_back('\'');
			cambios.push_back(s);
			cout << s << " ";
		}
		
	}

	virtual void imprimir_all_donas(){
		cout<<"centro: "<<origin.x<<", "<<
			origin.y<<", "<<origin.z<<endl<<endl;
	}
	
	virtual void rotateEverithing(){
		cout<<"Rotando dona final\n";
		for (int k = 0; k < 90; k++) //90 grados
		{
			for(int i = 0; i < cubitos.size(); i++){
				
				cubitos[i].move(glm::vec3(1.0,0.0,0.0),shader,degree);
			}

			dibujar_rubik();
			//esperamos un momento para que al momento de girar, se va de forma mas clara
			
			
			this_thread::sleep_for(chrono::nanoseconds(90000));
			
			//cout<<"Ya espero\n";
		}
	}

	virtual void rotate_z(float dg){
		for(int i = 0; i < cubitos.size(); i++){
			
			cubitos[i].move(glm::vec3(1.0,0.0,0.0),shader,dg);
		}
		dibujar_rubik();
	}
	virtual void rotate_z_noDraw(float dg){
		for(int i = 0; i < cubitos.size(); i++){
			cubitos[i].move(glm::vec3(1.0,0.0,0.0),shader,dg);
		}
	}
	virtual void translate_z(float distance){

		for(int i = 0; i < cubitos.size(); i++){

				cubitos[i].translate(glm::vec3(0.0f,0.0f,distance),shader);
		}
		dibujar_rubik();
	}
	virtual void translate_noDraw(glm::vec3 distance){
		for(int i = 0; i < cubitos.size(); i++){

			cubitos[i].translate(distance,shader);
		}			
	}

};
		
//----------------------------------------------------------------------------------------------------------		
class DonaRecursiva : public Dona{
	private:
		glm::vec3 centro;
		glm::vec3 centro_hijo;
		float dim;
		int orientacion;
		int depth;
	public:
		Dona* donaHija = nullptr;
		DonaRecursiva(GLFWwindow* _window,Shader* _shader,float dimension, int pos, glm::vec3 origen, int _depth):
			Dona(_window,_shader, dimension, pos, origen){
			depth = _depth;
			centro = origen;
			//Borrar cubito recursivo
			if(pos == 0)//z front
			{
				cubitos.erase(cubitos.begin()+6);
				centro_hijo = cubePositions[6];
			}
			else if(pos == 1)//y up}
			{
				cubitos.erase(cubitos.begin()+1);
				centro_hijo = cubePositions[1];
			}
			else if(pos == 2)//}z back
			{
				cubitos.erase(cubitos.begin()+19);
				centro_hijo = cubePositions[19];
			}
			else if(pos == 3)//y down
			{
				cubitos.erase(cubitos.begin()+24);
				centro_hijo = cubePositions[24];
			}

			//Adaptar atributos para el hijo
			dim = dimension;
			orientacion = pos; //0->1->2->3->0->...
			
			generarBuffers();
			//Crear hijo
			if(depth == 1){
				//cout<<"hijo 1\n";
				this->donaHija = new Dona(_window,_shader, dim/3, (pos+1)%4, centro_hijo);
			}else{
				//cout<<"hijo n\n";
				this->donaHija = new DonaRecursiva(_window,_shader, dim/3, (pos+1)%4, centro_hijo, depth-1);
			}
		}
		
		glm::vec3 getCurrentCenter(){
			Cubo* target;
			glm::vec3 r(0.0f,0.0f,0.0f);
			if(orientacion == 0 || orientacion == 1)
				target = &cubitos[13];
			else if(orientacion == 2 || orientacion == 3)
				target = &cubitos[14];

			for(int i = 0; i < 36; i++){
				r.x += target->vertices_centros[i].x;
				r.y += target->vertices_centros[i].y;
				r.z += target->vertices_centros[i].z;
			}
			r.x/=36;
			r.y/=36;
			r.z/=36;
			return r;
		}
		void generarBuffers() {
			for (int i = 0; i < cubitos.size(); i++){
				cubitos[i].generarBuffers();
				if(depth == 4)
					cubitos[i].cargar_texturas(shader, "\\OwnProjects\\Final_1\\textures\\cristal.jpg");
				else if(depth == 3)
					cubitos[i].cargar_texturas(shader, "\\OwnProjects\\Final_1\\textures\\tierra.jpg");
				else if(depth == 2)
					cubitos[i].cargar_texturas(shader, "\\OwnProjects\\Final_1\\textures\\cristal2.jpg");
				else if(depth == 1)
					cubitos[i].cargar_texturas(shader, "\\OwnProjects\\Final_1\\textures\\tierra2.jpg");	
				else
					cubitos[i].cargar_texturas(shader, "\\OwnProjects\\Final_1\\textures\\cristal2.jpg");
			}
		}
		void imprimir_all_donas(){
			cout<<"dimension: "<<dim<<endl<<
			"pos: "<<orientacion<<endl<<"centro: "<<centro.x<<", "<<
			centro.y<<", "<<centro.z<<endl<<"depth: "<<depth<<endl
			<<"Hijo: "<<!(donaHija==nullptr)<<endl<<endl;

			if(this->donaHija) this->donaHija->imprimir_all_donas();
		}
		void dibujar_rubik(){
			for (int i = 0; i < cubitos.size(); i++){
				cubitos[i].activar_texturas();
				cubitos[i].dibujar_cubito(shader);
			}
				
			if(this->donaHija != nullptr) donaHija->dibujar_rubik();
			
			//cout<<"Dona "<<centro.x<<", "<<centro.y<<", "<<centro.z<<" ya no tiene hijos\n";
		}
		void rotateEverithing(){
			cout<<"Rotando dona n\n";
			for (int k = 0; k < 90; k++) //90 grados
			{
				for(int i = 0; i < cubitos.size(); i++){
					cubitos[i].move(glm::vec3(1.0,0.0,0.0),shader,degree);
				}
				dibujar_rubik();
				//esperamos un momento para que al momento de girar, se va de forma mas clara
				
				std::this_thread::sleep_for(chrono::nanoseconds(900000));
				
				//cout<<"Ya espero\n";
			}
			if(donaHija) donaHija->rotateEverithing();
		}
		void rotate_z(float dg){
			for(int i = 0; i < cubitos.size(); i++){
					cubitos[i].move(glm::vec3(1.0,0.0,0.0),shader,dg);
			}
			dibujar_rubik();
			if(donaHija) donaHija->rotate_z(dg);
		}
		void rotate_z_noDraw(float dg){
			for(int i = 0; i < cubitos.size(); i++){
				cubitos[i].move(glm::vec3(1.0,0.0,0.0),shader,dg);
			}
			if(donaHija) donaHija->rotate_z_noDraw(dg);
		}
		void translate_z(float distance){
			for(int i = 0; i < cubitos.size(); i++){
				cubitos[i].translate(glm::vec3(0.0f,0.0f,distance),shader);
			}
			dibujar_rubik();
			if(donaHija) donaHija->translate_z(distance);
		}
		void translate_noDraw(glm::vec3 distance){
			for(int i = 0; i < cubitos.size(); i++){
				cubitos[i].translate(distance,shader);
			}
			if(donaHija) donaHija->translate_noDraw(distance);			
		}
		glm::vec3 getCentro(){
			return centro;
		}
		
		~DonaRecursiva(){
			delete donaHija;
		}
	
};
			
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// camera
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 20.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f;

float angle = 0.0f;
float angle1 = 0.0f;
float angle2 = 0.0f;
float angle3 = 0.0f;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

bool rotartodo = false;
//---------------------------------------------------------------------------------------------------------------------------------------

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Animacion Final", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGL(glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

	fs::path p = fs::current_path();
	int levels_path = 1;
	fs::path p_current; 
	p_current = p.parent_path();	
	std::string vs_path , fs_path;
	
	std::stringstream ss;
	ss << std::quoted( p_current.string() );
	std::string out;
	ss >> std::quoted(out);

	vs_path = out + "\\OwnProjects\\Final_1\\camera.vs";
	fs_path = out + "\\OwnProjects\\Final_1\\camera.fs";

    Shader ourShader(vs_path.c_str(), fs_path.c_str());

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader.use();

	DonaRecursiva MyCube(window,&ourShader,2.0f,0,glm::vec3(0.0f,0.0f,0.0f),4);
	glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    ourShader.setMat4("projection", projection);

	MyCube.imprimir_all_donas();

	float firstFrame = static_cast<float>(glfwGetTime());
	long long counter = 0;
	int step = 153;

    while (!glfwWindowShouldClose(window))
    {
		float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

		//first
		if(counter < step){
			MyCube.translate_z(0.05f);
		}
		else if(counter < step*2){
			MyCube.translate_noDraw(glm::vec3(0.0f,0.0f,-step*0.05f -2.0f));
			MyCube.rotate_z_noDraw(90.0f/step);
			MyCube.translate_noDraw(glm::vec3(0.0f,0.0f,step*0.05f +2.0f));
			MyCube.dibujar_rubik();
		}
		else if(counter < step*3){
			MyCube.translate_z(-0.0035f);
		}
		//second
		else if(counter < step*4){
			MyCube.translate_z(0.05f/4.0f);
		}
		else if(counter < step*5){
			MyCube.translate_noDraw(glm::vec3(0.0f,0.0f,-step*0.05f -0.0035f -2.0f));
			MyCube.rotate_z_noDraw(90.0f/step);
			MyCube.translate_noDraw(glm::vec3(0.0f,0.0f,step*0.05f +0.0035f  +2.0f));
			MyCube.dibujar_rubik();
		}
		//tp
		else if(counter == step*5){
			glm::vec3 c = MyCube.getCurrentCenter();
			cout<<"center: "<<c.x<<" "<<c.y<<" "<<c.z<<endl;
			MyCube.translate_noDraw(glm::vec3(-c.x,-c.y,-c.z));
			MyCube.rotate_z_noDraw(180.0f);
			MyCube.dibujar_rubik();
			counter = -1;
		}
		

		

		
				
        processInput(window);

		if(rotartodo){
			MyCube.rotate_z(deltaTime);
			rotartodo = false;
		}

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
        // activate shader
        ourShader.use();
		//cout<<"shader\n";

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos+cameraFront, cameraUp);
        ourShader.setMat4("view", view);
		//cout<<"lookAt\n";

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		MyCube.dibujar_rubik();
		//cout<<"dibujar\n";
		glfwSwapBuffers(window);
		glfwPollEvents();
		
		// cout <<"enMovimiento: "<<enMovimiento<<endl;

		counter++;
	}
	// optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    
    MyCube.deleteBuffers();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}


void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Invertido
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
float tempRadius = 3.0f;
float tempAngle = 90.0f;
float tempAngle2= 0.0f;
float xad = 0.1f,zad = 0.0, aux = cameraPos.x;
float totalStep = 4.0f;
float currentStep = 2.0f;

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 0.05;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
		//cameraPos += cameraSpeed*glm::vec3(0,0,-1);
		cameraPos += cameraSpeed*cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
		//cameraPos -= cameraSpeed*glm::vec3(0,0,-1);
		cameraPos -= cameraSpeed*cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
		//cameraPos -= glm::normalize(glm::cross(glm::vec3(0,0,-1), cameraUp)) * cameraSpeed;
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
		//cameraPos += glm::normalize(glm::cross(glm::vec3(0,0,-1), cameraUp)) * cameraSpeed;
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        tempAngle += 15*cameraSpeed;
        float cameraX = tempRadius * cos(glm::radians( tempAngle));
        float cameraZ = tempRadius * sin(glm::radians( tempAngle));
        cameraPos = glm::vec3(cameraX, cameraPos.y, cameraZ);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        tempAngle -= 15*cameraSpeed;
        float cameraX = tempRadius * cos(glm::radians( tempAngle));
        float cameraZ = tempRadius * sin(glm::radians( tempAngle));
        cameraPos = glm::vec3(cameraX, cameraPos.y, cameraZ);
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    {
        tempAngle2 += 15*cameraSpeed;
        float cameraY = tempRadius * cos(glm::radians( tempAngle2));
        float cameraZ = tempRadius * sin(glm::radians( tempAngle2));
        cameraPos = glm::vec3(cameraPos.x, cameraY, cameraZ);
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    {
        tempAngle2 -= 15*cameraSpeed;
        float cameraY = tempRadius * cos(glm::radians( tempAngle2));
        float cameraZ = tempRadius * sin(glm::radians( tempAngle2));
        cameraPos = glm::vec3(cameraPos.x, cameraY, cameraZ);
    }
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		rotartodo = true;
	}
    
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
