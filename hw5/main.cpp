/*
	Ryan Barber
	CISC 440
	HW 5
*/


// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <unordered_set>
#include <vector>
#include <iostream>
#include <string>
#include <time.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/text2D.hpp>
#include <common/vboindexer.hpp>

// these along with Model matrix make MVP transform

glm::mat4 Projection;
glm::mat4 View;

// some globals necessary to get information to shaders

GLuint MatrixID;
GLuint vertexbuffer;
GLuint colorbuffer;

//for object
GLuint objMatrixID;
GLuint Texture;
GLuint TextureID;
GLuint objvertexbuffer;
GLuint objuvbuffer;
GLuint ViewMatrixID;
GLuint ModelMatrixID;
	// Read our .obj file
std::vector<glm::vec3> vertices;
std::vector<glm::vec2> uvs;
std::vector<glm::vec3> normals;

//Bad qbert
GLuint Texture2;
GLuint badvertexbuffer;
GLuint baduvbuffer;
std::vector<glm::vec3> badvertices;
std::vector<glm::vec2> baduvs;
std::vector<glm::vec3> badnormals;

//for text
std::vector<unsigned short> indices;


void draw_triangle(glm::mat4, float, float, float);
void draw_right_triangle(glm::mat4, float, float, float);
void draw_square(glm::mat4, float, float, float);
void draw_cube(glm::mat4, float, float, float);

void draw_pyramid(glm::mat4);
void draw_qbert(glm::mat4, glm::vec3);
void draw_bad_qbert(glm::mat4, glm::vec3);
void getPath(glm::vec3 *, glm::vec3*);
void reset_faces();
void change_face(int, int);
void reset_game();
bool check_cubes();
void set_control_points_t(glm::vec3*, glm::vec3*, int*, glm::vec3*);
void set_control_points_y(glm::vec3*, glm::vec3*, int*, glm::vec3*);
void set_control_points_g(glm::vec3*, glm::vec3*, int*, glm::vec3*);
void set_control_points_h(glm::vec3*, glm::vec3*, int*, glm::vec3*);
void move_bad_qbert();
void lost_life();
bool check_collision();
void next_level();

//struct for cube info
struct Cube{
	int idx;
	int level;
	int set_idx;
	int color_level_idx;
};

//qbert info set to inital position on top cube
glm::vec3 initial_pos = glm::vec3(4.9497f, 7.40f, -4.2426f);
glm::vec3 qbert_pos = initial_pos;
int qbert_cube = 0;
bool is_moving = false;

//bad qbert 
glm::vec3 bad_initial_pos = glm::vec3(0.7071f, 1.4f, 0.0f);
glm::vec3 bad_qbert_pos = bad_initial_pos;
int bad_qbert_cube = 21;
bool bad_is_moving = false;


//array of move sets - used instead of storing in each struct
std::unordered_set<char> moves[7] = {	{'g','h'},		//top 0
					{'y','h','g'},		//left edge 1
					{'t','g','h'},		//right edge 2
					{'t','y','g','h'},	//middle 3
					{'t','y'},		//bottom 4
					{'y'},			//bottom left corner 5
					{'t'},			//bottom right corner 6
				};
				
//array of color maps for levels
float color_map[4][3] = { {1.0, 0.0, 0.0}, {1.0, 1.0, 0.0}, {0.0, 1.0, 0.0} , {0.5,0.5,0.5}};
					
//array of cube structs
Cube pyramid[28] = {	{0,1,0,1}, {1,2,1,0}, {2,2,2,0}, {3,3,1,0}, {4,3,3,0}, {5,3,2,0},
			{6,4,1,0}, {7,4,3,0}, {8,4,3,0}, {9,4,2,0}, {10,5,1,0}, {11,5,3,0},
			{12,5,3,0}, {13,5,3,0}, {14,5,2,0}, {15,6,1,0}, {16,6,3,0}, {17,6,3,0},
			{18,6,3,0}, {19,6,3,0}, {20,6,2,0}, {21,7,5,0}, {22,7,4,0}, {23,7,4,0},
			{24,7,4,0}, {25,7,4,0}, {26,7,4,0}, {27,7,6,0}
			
			};
			
// control points for Catmull Rom
glm::vec3 controlPoints[6];
// array of line points
glm::vec3 path[30];
float angle = 0;

//bad qbert path
glm::vec3 bad_controlPoints[6];
glm::vec3 bad_path[30];
float bad_angle = 0;

// int for game
int game_level = 1;
int lives = 3;

//bool for win state at end
bool win = false;

//strings on screen
char level_string[32];
char control_string[] = "Controls: ";
char ctrl_str1[] = "t -up, left";
char ctrl_str2[] = "y -up, right";
char ctrl_str3[] = "g -down, left";
char ctrl_str4[] = "h -down, right";
char win_str[] = "WINNER";
char lives_string[32];
char quit_str[] = "Esc to quit";
char replay_str[] = "Space to reset";

// Draw pyramid of cubes like qbert game
// each cube is 1x1x1, rotated 45deg, and translated accordingly
void draw_pyramid(glm::mat4 Model)
{
	//ratation matrix
	glm::mat4 RY = glm::rotate((float) ((-0.25)*M_PI), glm::vec3(0.0f, 1.0f, 0.0f));
	
	//draw cube- i do manually
	
	// 0
	draw_cube(Model * glm::translate(glm::vec3(4.9497f, 6.0f, -4.2426f)) * RY, color_map[pyramid[0].color_level_idx][0], 
			color_map[pyramid[0].color_level_idx][1], color_map[pyramid[0].color_level_idx][2]);
	// 1
	draw_cube(Model * glm::translate(glm::vec3(4.2426f, 5.0f, -3.5355f)) * RY, color_map[pyramid[1].color_level_idx][0], 
			color_map[pyramid[1].color_level_idx][1], color_map[pyramid[1].color_level_idx][2]);
	//2
	draw_cube(Model * glm::translate(glm::vec3(5.6568f, 5.0f, -3.5355f)) * RY, color_map[pyramid[2].color_level_idx][0], 
			color_map[pyramid[2].color_level_idx][1], color_map[pyramid[2].color_level_idx][2]);
	//3
	draw_cube(Model * glm::translate(glm::vec3(3.5355f, 4.0f, -2.8284f)) * RY, color_map[pyramid[3].color_level_idx][0], 
			color_map[pyramid[3].color_level_idx][1], color_map[pyramid[3].color_level_idx][2]);
	//4
	draw_cube(Model * glm::translate(glm::vec3(4.9497f, 4.0f, -2.8284f)) * RY, color_map[pyramid[4].color_level_idx][0], 
			color_map[pyramid[4].color_level_idx][1], color_map[pyramid[4].color_level_idx][2]);
	//5
	draw_cube(Model * glm::translate(glm::vec3(6.3639f, 4.0f, -2.8284f)) * RY, color_map[pyramid[5].color_level_idx][0], 
			color_map[pyramid[5].color_level_idx][1], color_map[pyramid[5].color_level_idx][2]);
	//6
	draw_cube(Model * glm::translate(glm::vec3(2.8284f, 3.0f, -2.1213f)) * RY, color_map[pyramid[6].color_level_idx][0], 
			color_map[pyramid[6].color_level_idx][1], color_map[pyramid[6].color_level_idx][2]);
	//...
	draw_cube(Model * glm::translate(glm::vec3(4.2426f, 3.0f, -2.1213f)) * RY, color_map[pyramid[7].color_level_idx][0], 
			color_map[pyramid[7].color_level_idx][1], color_map[pyramid[7].color_level_idx][2]);
			
	draw_cube(Model * glm::translate(glm::vec3(5.6568f, 3.0f, -2.1213f)) * RY, color_map[pyramid[8].color_level_idx][0], 
			color_map[pyramid[8].color_level_idx][1], color_map[pyramid[8].color_level_idx][2]);
			
	draw_cube(Model * glm::translate(glm::vec3(7.071f, 3.0f, -2.1213f)) * RY, color_map[pyramid[9].color_level_idx][0], 
			color_map[pyramid[9].color_level_idx][1], color_map[pyramid[9].color_level_idx][2]);
			
	draw_cube(Model * glm::translate(glm::vec3(2.1213f, 2.0f, -1.4142f)) * RY, color_map[pyramid[10].color_level_idx][0], 
			color_map[pyramid[10].color_level_idx][1], color_map[pyramid[10].color_level_idx][2]);
			
	draw_cube(Model * glm::translate(glm::vec3(3.5355f, 2.0f, -1.4142f)) * RY, color_map[pyramid[11].color_level_idx][0], 
			color_map[pyramid[11].color_level_idx][1], color_map[pyramid[11].color_level_idx][2]);
			
	draw_cube(Model * glm::translate(glm::vec3(4.9497f, 2.0f, -1.4142f)) * RY, color_map[pyramid[12].color_level_idx][0], 
			color_map[pyramid[12].color_level_idx][1], color_map[pyramid[12].color_level_idx][2]);
			
	draw_cube(Model * glm::translate(glm::vec3(6.3639f, 2.0f, -1.4142f)) * RY, color_map[pyramid[13].color_level_idx][0], 
			color_map[pyramid[13].color_level_idx][1], color_map[pyramid[13].color_level_idx][2]);
			
	draw_cube(Model * glm::translate(glm::vec3(7.7781f, 2.0f, -1.4142f)) * RY, color_map[pyramid[14].color_level_idx][0], 
			color_map[pyramid[14].color_level_idx][1], color_map[pyramid[14].color_level_idx][2]);
			
	draw_cube(Model * glm::translate(glm::vec3(1.4142f, 1.0f, -0.7071f)) * RY, color_map[pyramid[15].color_level_idx][0], 
			color_map[pyramid[15].color_level_idx][1], color_map[pyramid[15].color_level_idx][2]);
			
	draw_cube(Model * glm::translate(glm::vec3(2.8284f, 1.0f, -0.7071f)) * RY, color_map[pyramid[16].color_level_idx][0], 
			color_map[pyramid[16].color_level_idx][1], color_map[pyramid[16].color_level_idx][2]);
			
	draw_cube(Model * glm::translate(glm::vec3(4.2426f, 1.0f, -0.7071f)) * RY, color_map[pyramid[17].color_level_idx][0], 
			color_map[pyramid[17].color_level_idx][1], color_map[pyramid[17].color_level_idx][2]);
			
	draw_cube(Model * glm::translate(glm::vec3(5.6568f, 1.0f, -0.7071f)) * RY, color_map[pyramid[18].color_level_idx][0], 
			color_map[pyramid[18].color_level_idx][1], color_map[pyramid[18].color_level_idx][2]);
			
	draw_cube(Model * glm::translate(glm::vec3(7.071f, 1.0f, -0.7071f)) * RY, color_map[pyramid[19].color_level_idx][0], 
			color_map[pyramid[19].color_level_idx][1], color_map[pyramid[19].color_level_idx][2]);
			
	draw_cube(Model * glm::translate(glm::vec3(8.4852f, 1.0f, -0.7071f)) * RY, color_map[pyramid[20].color_level_idx][0], 
			color_map[pyramid[20].color_level_idx][1], color_map[pyramid[20].color_level_idx][2]);
			
	draw_cube(Model * glm::translate(glm::vec3(0.7071f, 0.0f, 0.0f)) * RY, color_map[pyramid[21].color_level_idx][0], 
			color_map[pyramid[21].color_level_idx][1], color_map[pyramid[21].color_level_idx][2]);
			
	draw_cube(Model * glm::translate(glm::vec3(2.1213f, 0.0f, 0.0f)) * RY, color_map[pyramid[22].color_level_idx][0], 
			color_map[pyramid[22].color_level_idx][1], color_map[pyramid[22].color_level_idx][2]);
			
	draw_cube(Model * glm::translate(glm::vec3(3.5355f, 0.0f, 0.0f)) * RY, color_map[pyramid[23].color_level_idx][0], 
			color_map[pyramid[23].color_level_idx][1], color_map[pyramid[23].color_level_idx][2]);
			
	draw_cube(Model * glm::translate(glm::vec3(4.9497f, 0.0f, 0.0f)) * RY, color_map[pyramid[24].color_level_idx][0], 
			color_map[pyramid[24].color_level_idx][1], color_map[pyramid[24].color_level_idx][2]);
			
	draw_cube(Model * glm::translate(glm::vec3(6.3639f, 0.0f, 0.0f)) * RY, color_map[pyramid[25].color_level_idx][0], 
			color_map[pyramid[25].color_level_idx][1], color_map[pyramid[25].color_level_idx][2]);
			
	draw_cube(Model * glm::translate(glm::vec3(7.7781f, 0.0f, 0.0f)) * RY, color_map[pyramid[26].color_level_idx][0], 
			color_map[pyramid[26].color_level_idx][1], color_map[pyramid[26].color_level_idx][2]);
			
	draw_cube(Model * glm::translate(glm::vec3(9.1923f, 0.0f, 0.0f)) * RY, color_map[pyramid[27].color_level_idx][0], 
			color_map[pyramid[27].color_level_idx][1], color_map[pyramid[27].color_level_idx][2]);

	
}
//----------------------------------------------------------------------------

// 1x1x1 cube centered on (0, 0, 0)
void draw_cube(glm::mat4 Model, float r, float g, float b)
{
  // +Z, -Z
  
  draw_square(Model * glm::translate(glm::vec3(0.0f, 0.0f, +0.5f)), 0.0f, 1.0f, 1.0f); //FRONT FACE
  draw_square(Model * glm::translate(glm::vec3(0.0f, 0.0f, -0.5f)), 0.5*r, 0.5*g, 0.5*b);

  // +X, -X

  glm::mat4 RY = glm::rotate((float) (0.5*M_PI),
                 glm::vec3(    0.0f,
                    1.0f,
                    0.0f));

  draw_square(Model * glm::translate(glm::vec3(+0.5f, 0.0f, 0.0f)) * RY, 0.0f, 0.0f, 1.0f); //RIGHT FACE
  draw_square(Model * glm::translate(glm::vec3(-0.5f, 0.0f, 0.0f)) * RY, 0.5*g, 0.5*b, 0.5*r);

  // +Y, -Y

  glm::mat4 RX = glm::rotate((float) (0.5*M_PI),
                 glm::vec3(    1.0f,
                    0.0f,
                    0.0f));

  draw_square(Model * glm::translate(glm::vec3(0.0f, +0.5f, 0.0f)) * RX, r, g, b); // TOP FACE
  draw_square(Model * glm::translate(glm::vec3(0.0f, -0.5f, 0.0f)) * RX, 0.5*b, 0.5*r, 0.5*g);

}
//----------------------------------------------------------------------------

// 1 x 1 square centered on (0, 0)
void draw_square(glm::mat4 Model, float r, float g, float b)
{
  glm::mat4 M = glm::scale(glm::vec3(-0.5f, -0.5f, 0.0f));
  glm::mat4 M2 = glm::scale(glm::vec3(0.5f, 0.5f, 0.0f));

  //  draw_right_triangle(Model * M, 1.0-r, 1.0-g, 1.0-b);
  draw_right_triangle(Model * M, r, g, b);
  draw_right_triangle(Model * M2, r, g, b);
}
//----------------------------------------------------------------------------

// with shear, bottom-left at (-1, -1), bottom-right at (1, -1),
// top-right at (1, 1)
void draw_right_triangle(glm::mat4 Model, float r, float g, float b)
{
  glm::mat4 S = glm::shearX3D (glm::mat4(1.0f), 0.5f, 0.0f);
  glm::mat4 T = glm::translate(glm::vec3(-1.0f, 1.0f, 0.0f));
  
  draw_triangle(Model * glm::inverse(T) * S * T, r, g, b);
}
//----------------------------------------------------------------------------

// bottom-left at (-1, -1), bottom-right at (1, -1),
// top at (0, 1)
// Draw triangle with particular modeling transformation and color (r, g, b) (in range [0, 1])
// Refers to globals in section above (but does not change them)
void draw_triangle(glm::mat4 Model, float r, float g, float b)
{
  // Our ModelViewProjection : multiplication of our 3 matrices
  glm::mat4 MVP = Projection * View * Model;

  // make this transform available to shaders  
  glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

  // 1st attribute buffer : vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glVertexAttribPointer(0,                  // attribute. 0 to match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
            );
  
  // all vertices same color

  GLfloat g_color_buffer_data[] = { 
    r, g, b,
    r, g, b,
    r, g, b,
  };
  
  glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
  
  // 2nd attribute buffer : colors
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
  glVertexAttribPointer(1,                                // attribute. 1 to match the layout in the shader.
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
            );

  // Draw the triangle !
  glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle
  
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
}
//----------------------------------------------------------------------------

//draw qbert
void draw_qbert(glm::mat4 Model, glm::vec3 pos){
    glm::mat4 RY = glm::rotate((float) ((angle)*M_PI), glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::translate(Model, pos) * RY;

    // Our ModelViewProjection : multiplication of our 3 matrices
    glm::mat4 MVP = Projection * View * Model;

   // make this transform available to shaders  
   glUniformMatrix4fv(objMatrixID, 1, GL_FALSE, &MVP[0][0]);
   glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &View[0][0]);
   glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &Model[0][0]);
  
   // Bind our texture in Texture Unit 0
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, Texture);
   // Set our "myTextureSampler" sampler to use Texture Unit 0
   glUniform1i(TextureID, 0);

   // 1rst attribute buffer : vertices
   glEnableVertexAttribArray(0);
   glBindBuffer(GL_ARRAY_BUFFER, objvertexbuffer);
   glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

   // 2nd attribute buffer : UVs
   glEnableVertexAttribArray(1);
   glBindBuffer(GL_ARRAY_BUFFER, objuvbuffer);
   glVertexAttribPointer(
		1,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);
		

   // Draw the triangle !
   glDrawArrays(GL_TRIANGLES, 0, vertices.size() );
		
   glDisableVertexAttribArray(1);
   glDisableVertexAttribArray(0);
}
//-------------------------------------------------------------------------

//draw bad qbert
void draw_bad_qbert(glm::mat4 Model, glm::vec3 pos){
  glm::mat4 RY = glm::rotate((float) ((bad_angle)*M_PI), glm::vec3(0.0f, 1.0f, 0.0f));
  Model = glm::translate(Model, pos) * RY;

    // Our ModelViewProjection : multiplication of our 3 matrices
    glm::mat4 MVP = Projection * View * Model;

   // make this transform available to shaders  
   glUniformMatrix4fv(objMatrixID, 1, GL_FALSE, &MVP[0][0]);
   glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &View[0][0]);
   glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &Model[0][0]);
  
   // Bind our texture in Texture Unit 0
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, Texture2);
   // Set our "myTextureSampler" sampler to use Texture Unit 0
   glUniform1i(TextureID, 0);

   // 1rst attribute buffer : vertices
   glEnableVertexAttribArray(0);
   glBindBuffer(GL_ARRAY_BUFFER, badvertexbuffer);
   glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

   // 2nd attribute buffer : UVs
   glEnableVertexAttribArray(1);
   glBindBuffer(GL_ARRAY_BUFFER, baduvbuffer);
   glVertexAttribPointer(
		1,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);
		

   // Draw the triangle !
   glDrawArrays(GL_TRIANGLES, 0, badvertices.size() );
		
   glDisableVertexAttribArray(1);
   glDisableVertexAttribArray(0);

}
//----------------------------------------------------------------

//catmull rom the jump path
void getPath(glm::vec3 *cp, glm::vec3 *mypath){
	// variable for path array index
	int path_idx = 0;
	
	//indices for control poins array
	int p0, p1, p2, p3;
	
	//matricies for catmull rom equation; need x,y,z for each point to get vec3 output
	glm::vec4 p_matX;
	glm::vec4 p_matY;
	glm::vec4 p_matZ;
	glm::vec4 t_mat;
	glm::mat4 matrix2 = glm::mat4(glm::vec4(0,-1,2,-1),	//each vec4 is a col (transposed from whats shown in slides)
				       glm::vec4(2,0,-5,3),
				       glm::vec4(0,1,4,-3),
				       glm::vec4(0,0,-1,1) );

		// loop through t vals
		for(float i = 0.0; i < 2.9f; i+=0.1f){
			//set control point indicies
			p1 = (int)i + 1;
			p2 = (p1 + 1);
			p3 = (p2 + 1);
			p0 = p1 -1; 
			
			p_matX[0] = cp[p0].x;
			p_matX[1] = cp[p1].x;	//get x values matrix
			p_matX[2] = cp[p2].x;
			p_matX[3] = cp[p3].x;
			
			p_matY[0] = cp[p0].y;
			p_matY[1] = cp[p1].y;	//get y values matrix
			p_matY[2] = cp[p2].y;
			p_matY[3] = cp[p3].y;
		
			p_matZ[0] = cp[p0].z;
			p_matZ[1] = cp[p1].z;	//get z values matrix
			p_matZ[2] = cp[p2].z;
			p_matZ[3] = cp[p3].z;
			
			//get time matrix
			t_mat[0] = 1;
			t_mat[1] = (float)(i - (int)i);
			t_mat[2] = t_mat[1] * t_mat[1];
			t_mat[3] = t_mat[2] * t_mat[1];
			
			//get output of matrix multiplication
			glm::vec4 outX = 0.5f * t_mat * matrix2 * p_matX;
			glm::vec4 outY = 0.5f * t_mat * matrix2 * p_matY;
			glm::vec4 outZ = 0.5f * t_mat * matrix2 * p_matZ;
			
			//sum terms of the output matrix (vec4 1x4) for final answer
			float outx = outX.x + outX.y + outX.z + outX.w;
			float outy = outY.x + outY.y + outY.z + outY.w;
			float outz = outZ.x + outZ.y + outZ.z + outZ.w;
			
			//add to path array
			mypath[path_idx] = glm::vec3(outx, outy, outz);
			path_idx ++;
		}
}
//-------------------------------------------------------------------

//check change color of cube face
void reset_faces(){
  pyramid[0].color_level_idx = 1;
  for(int i = 1; i < 28; i++){
    pyramid[i].color_level_idx = 0;
  }
}
//------------------------------------

void change_face(int cube_idx, int level){
  if(pyramid[cube_idx].color_level_idx < level){
      pyramid[cube_idx].color_level_idx += 1;
  }
}
//--------------------------------------------

//reset game
//qbert to top and cube faces reset
void reset_game(){
  qbert_pos = initial_pos;
  bad_qbert_pos = bad_initial_pos;
  reset_faces();
  is_moving = false;
  bad_is_moving = false;
  lives = 3;
  qbert_cube = 0;
  bad_qbert_cube = 21;
  game_level = 1;
  win = false;
}
//----------------------------

//reset upon lost life
void lost_life(){
  qbert_pos = initial_pos;
  bad_qbert_pos = bad_initial_pos;
  is_moving = false;
  bad_is_moving = false;
  qbert_cube = 0;
  bad_qbert_cube = 21;
}
//-------------------------------

//check for all cubes done
bool check_cubes(){
  for(int i = 0; i < 28; i++){
    if(pyramid[i].color_level_idx != game_level){
      return false;
    }
  }
  win = true;
  return true;
}
//------------------------------

//next level
void next_level(){
  lost_life();
  reset_faces();
  lives = 3;
  game_level += 1;
  win = false;
}
//------------------------------

//functions to set control points for keys
void set_control_points_t(glm::vec3 *cp, glm::vec3 *qbert, int *cube, glm::vec3 *mypath){
  cp[0] = glm::vec3(qbert->x, qbert->y-0.5f, qbert->z);
  cp[1] = *qbert; //this is glm::vec3
  cp[2] = glm::vec3(qbert->x-0.2357f, qbert->y+1.333f, qbert->z-0.2357f);
  cp[3] = glm::vec3(qbert->x-0.566f, qbert->y+1.555f, qbert->z-0.4714f);
  cp[4] = glm::vec3(qbert->x-0.7071f, qbert->y+1.0f, qbert->z-0.7071f);
  cp[5] = glm::vec3(qbert->x-0.733f, qbert->y+0.5f, qbert->z-0.733f);
  getPath(cp, mypath); // smooth path set for jump
  *qbert = glm::vec3(qbert->x-0.7071f, qbert->y+1.0f, qbert->z-0.7071f);
  *cube = *cube - pyramid[*cube].level;
}
//---------------------------------------------------------
void set_control_points_y(glm::vec3 *cp, glm::vec3 *qbert, int *cube, glm::vec3 *mypath){
  cp[0] = glm::vec3(qbert->x, qbert->y-0.5f, qbert->z);
  cp[1] = *qbert; //this is glm::vec3
  cp[2] = glm::vec3(qbert->x+0.2357f, qbert->y+1.333f, qbert->z-0.2357f);
  cp[3] = glm::vec3(qbert->x+0.566f, qbert->y+1.555f, qbert->z-0.4714f);
  cp[4] = glm::vec3(qbert->x+0.7071f, qbert->y+1.0f, qbert->z-0.7071f);
  cp[5] = glm::vec3(qbert->x+0.733f, qbert->y+0.5f, qbert->z-0.733f);
  getPath(cp, mypath); // smooth path set for jump
  *qbert = glm::vec3(qbert->x+0.7071f, qbert->y+1.0f, qbert->z-0.7071f);
  *cube = *cube - pyramid[*cube].level + 1;
}
//---------------------------------------------------------
void set_control_points_g(glm::vec3 *cp, glm::vec3 *qbert, int *cube, glm::vec3 *mypath){
  cp[0] = glm::vec3(qbert->x, qbert->y-0.5f, qbert->z);
  cp[1] = *qbert; //this is glm::vec3
  cp[2] = glm::vec3(qbert->x-0.2357f, qbert->y+0.3333f, qbert->z+0.2357f);
  cp[3] = glm::vec3(qbert->x-0.566f, qbert->y-0.555f, qbert->z+0.4714f);
  cp[4] = glm::vec3(qbert->x-0.7071f, qbert->y-1.0f, qbert->z+0.7071f);
  cp[5] = glm::vec3(qbert->x-0.733f, qbert->y-1.5f, qbert->z+0.733f);
  getPath(cp, mypath); // smooth path set for jump
  *qbert = glm::vec3(qbert->x-0.7071f, qbert->y-1.0f, qbert->z+0.7071f);
  *cube = *cube + pyramid[*cube].level;
}
//---------------------------------------------------------
void set_control_points_h(glm::vec3 *cp, glm::vec3 *qbert, int *cube, glm::vec3 *mypath){
  cp[0] = glm::vec3(qbert->x, qbert->y-0.5f, qbert->z);
  cp[1] = *qbert; //this is glm::vec3
  cp[2] = glm::vec3(qbert->x+0.2357f, qbert->y+0.3333f, qbert->z+0.2357f);
  cp[3] = glm::vec3(qbert->x+0.566f, qbert->y-0.555f, qbert->z+0.4714f);
  cp[4] = glm::vec3(qbert->x+0.7071f, qbert->y-1.0f, qbert->z+0.7071f);
  cp[5] = glm::vec3(qbert->x+0.733f, qbert->y-1.5f, qbert->z+0.733f);
  getPath(cp, mypath); // smooth path set for jump
  *qbert = glm::vec3(qbert->x+0.7071f, qbert->y-1.0f, qbert->z+0.7071f);
  *cube = *cube + pyramid[*cube].level + 1;
}
//---------------------------------------------------------

//Bad qbert move
//Test bad qbert move;
void move_bad_qbert(){
  int move = rand() % 4;
  switch(move){
    case 0:
      if(moves[pyramid[bad_qbert_cube].set_idx].find('t') != moves[pyramid[bad_qbert_cube].set_idx].end()){
        set_control_points_t(bad_controlPoints, &bad_qbert_pos, &bad_qbert_cube, bad_path);
        bad_is_moving = true;
      }
      return;
    case 1:
      if(moves[pyramid[bad_qbert_cube].set_idx].find('y') != moves[pyramid[bad_qbert_cube].set_idx].end()){
        set_control_points_y(bad_controlPoints, &bad_qbert_pos, &bad_qbert_cube, bad_path);
        bad_is_moving = true;
      }
      return;
    case 2:
      if(moves[pyramid[bad_qbert_cube].set_idx].find('g') != moves[pyramid[bad_qbert_cube].set_idx].end()){
        set_control_points_g(bad_controlPoints, &bad_qbert_pos, &bad_qbert_cube, bad_path);
        bad_is_moving = true;
        bad_angle = -0.25;
      }
      return;
    case 3:
      if(moves[pyramid[bad_qbert_cube].set_idx].find('h') != moves[pyramid[bad_qbert_cube].set_idx].end()){
        set_control_points_h(bad_controlPoints, &bad_qbert_pos, &bad_qbert_cube, bad_path);
        bad_is_moving = true;
        bad_angle = 0.25;
      }
      return;
  }
}
//-------------------------------------------------------------

// check collision between good and bad qbert
bool check_collision(){
  if(qbert_cube == bad_qbert_cube){
    lives -= 1;
    if(lives == 0){
      reset_game();
      return true;
    }
    lost_life();
    return true;
  }
  return false;
}
//---------------------------------------------------------

//key press handler
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  //Key Map
  // t = up left
  // y = up right
  // g = down left
  // h = down right

  if (key == GLFW_KEY_T && (action == GLFW_PRESS || action == GLFW_REPEAT) && is_moving == false) {
  	if(moves[pyramid[qbert_cube].set_idx].find('t') != moves[pyramid[qbert_cube].set_idx].end()){
  		//set control points
      set_control_points_t(controlPoints, &qbert_pos, &qbert_cube, path);
      is_moving = true;
  	}
  }
  else if (key == GLFW_KEY_Y && (action == GLFW_PRESS || action == GLFW_REPEAT)&& is_moving == false){
  	if(moves[pyramid[qbert_cube].set_idx].find('y') != moves[pyramid[qbert_cube].set_idx].end()){
  		//set control points
      set_control_points_y(controlPoints, &qbert_pos, &qbert_cube, path);
      is_moving = true;
  	}
  }
  else if (key == GLFW_KEY_G && (action == GLFW_PRESS || action == GLFW_REPEAT)&& is_moving == false){
  	if(moves[pyramid[qbert_cube].set_idx].find('g') != moves[pyramid[qbert_cube].set_idx].end()){
  		//set control points
      set_control_points_g(controlPoints, &qbert_pos, &qbert_cube, path);
      is_moving = true;
      angle = -0.25;
  	}
  } 
  else if (key == GLFW_KEY_H && (action == GLFW_PRESS || action == GLFW_REPEAT)&& is_moving == false){
  	if(moves[pyramid[qbert_cube].set_idx].find('h') != moves[pyramid[qbert_cube].set_idx].end()){
  		//set control points
      set_control_points_h(controlPoints, &qbert_pos, &qbert_cube, path);
      is_moving = true;
      angle = 0.25;
  	}
  }
  else if (key == GLFW_KEY_SPACE && (action == GLFW_PRESS || action == GLFW_REPEAT)){
    reset_game();
  }
}
//----------------------------------------------------------------

int main( void )
{
  //seed random
  srand(time(0));


  // Initialise GLFW
  if( !glfwInit() )
    {
      fprintf( stderr, "Failed to initialize GLFW\n" );
      getchar();
      return -1;
    }
  
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 
  
  // Open a window and create its OpenGL context
  window = glfwCreateWindow( 1024, 768, "HW 5 - Simple Qbert", NULL, NULL);
  if( window == NULL ){
    fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
    getchar();
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  
  // Initialize GLEW
  glewExperimental = true; // Needed for core profile
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    getchar();
    glfwTerminate();
    return -1;
  }
  
  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
  
  // register all callbacks
  glfwSetKeyCallback(window, key_callback);
  
  // Darker blue background
  glClearColor(0.0f, 0.0f, 0.2f, 0.0f);
  
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);
  
  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);
  
  // Create and compile our GLSL program from the shaders
  GLuint programID = LoadShaders( "MultiColorSimpleTransform.vertexshader", "MultiColor.fragmentshader" );
  GLuint objprogramID = LoadShaders( "TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader");
  
  // Get a handle for our "MVP" uniform
  MatrixID = glGetUniformLocation(programID, "MVP");
  objMatrixID = glGetUniformLocation(objprogramID, "MVP");
  ViewMatrixID = glGetUniformLocation(objprogramID, "V");
  ModelMatrixID = glGetUniformLocation(objprogramID, "M");
  
  // Load the texture
  Texture = loadDDS("qbert1.dds");
  Texture2 = loadDDS("badqbert.dds");
	
  // Get a handle for our "myTextureSampler" uniform
  TextureID  = glGetUniformLocation(objprogramID, "mytexturesampler");
  
  bool res = loadOBJ("qbert.obj", vertices, uvs, normals);
  bool res2 = loadOBJ("qbert.obj", badvertices, baduvs, badnormals);

  // Load it into a VBO
  glGenBuffers(1, &objvertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, objvertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

  glGenBuffers(1, &objuvbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, objuvbuffer);
  glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

  // Load bad qbert into a VBO
  glGenBuffers(1, &badvertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, badvertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, badvertices.size() * sizeof(glm::vec3), &badvertices[0], GL_STATIC_DRAW);

  glGenBuffers(1, &baduvbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, baduvbuffer);
  glBufferData(GL_ARRAY_BUFFER, baduvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

  // Initialize our little text library with the Holstein font
	initText2D( "Holstein.DDS" );

  // Generate a buffer for the indices as well
	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

  // geometry of "template" triangle
  static const GLfloat g_vertex_buffer_data[] = { 
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f,  1.0f, 0.0f,
  };
  static const GLushort g_element_buffer_data[] = { 0, 1, 2 };

  glGenBuffers(1, &vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
  
  // handle for color information.  we don't set it here because it can change for each triangle

  glGenBuffers(1, &colorbuffer);
  
  //MVP MATRICIES
  // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
  Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
  
  // View looking near center of pyramid
  View = glm::lookAt(glm::vec3(4.9,8,10), glm::vec3(5,3.5,0), glm::vec3(0,1,0));
  //View = glm::lookAt(glm::vec3(-3,6,0), glm::vec3(5,5,-5), glm::vec3(0,1,0));
  
  // Model matrix -- changed for each triangle drawn
  glm::mat4 Model;
  

  //path index for jump path
  int path_idx = 0;

  int bad_path_idx = 0;

//#################### DRAW LOOP ##########################
  do{
    // Clear the screen
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Use obj shader
    glUseProgram(objprogramID);
    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    
    //draw qbert & bad qbert
    if(bad_is_moving){
      draw_bad_qbert(Model, bad_path[bad_path_idx]);
      bad_path_idx ++;
    } else {
      draw_bad_qbert(Model, bad_qbert_pos);
      move_bad_qbert();
    }

    //arbitrary point on bad qbert jump path to check for collision
    if(bad_path_idx == 20 && win == false){
      if(check_collision()){
        continue;
      }
    }
    if(bad_path_idx == 29){
      bad_path_idx = 0;
      bad_is_moving = false;
    }

  //#####################################################
    //if moving draw move path else use qbert pos
    if(is_moving){
      draw_qbert(Model, path[path_idx]);
      path_idx ++;
    } else {
      draw_qbert(Model, qbert_pos);
    }
    if(path_idx == 29){
      path_idx = 0;
      is_moving = false;
      change_face(qbert_cube, game_level);
    }
    
    glDisable(GL_CULL_FACE);
    
    glUseProgram(programID);
    draw_pyramid(Model);
    
    sprintf(level_string,"Level: %d", game_level );
    printText2D(level_string, 10, 500, 20);
    sprintf(lives_string, "Lives: %d", lives);
    printText2D(lives_string, 10, 450,20);
    printText2D(control_string, 500, 475, 20);
    printText2D(ctrl_str1, 500, 450, 20);
    printText2D(ctrl_str2, 500, 425, 20);
    printText2D(ctrl_str3, 500, 400, 20);
    printText2D(ctrl_str4, 500, 375, 20);
    printText2D(replay_str, 10, 400, 20);

    //CHECK NEXT LEVEL
    if(check_cubes()){
      //CHECK WIN GAME
      if(game_level == 3){
        printText2D(win_str, 10, 375, 30);
        printText2D(quit_str, 10, 350, 20);
      } else {
        next_level();
      }
    }

  
    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

  } // Check if the ESC key was pressed or the window was closed
  while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
     glfwWindowShouldClose(window) == 0 );
  
  // Cleanup VBO and shader
  glDeleteBuffers(1, &vertexbuffer);
  glDeleteBuffers(1, &objvertexbuffer);
  glDeleteBuffers(1, &objuvbuffer);
  glDeleteBuffers(1, &badvertexbuffer);
  glDeleteBuffers(1, &baduvbuffer);
  glDeleteBuffers(1, &elementbuffer);
  glDeleteProgram(programID);
  glDeleteProgram(objprogramID);
  glDeleteTextures(1, &Texture);
  glDeleteTextures(1,&Texture2);
  glDeleteVertexArrays(1, &VertexArrayID);
  
  // Delete the text's VBO, the shader and the texture
  cleanupText2D();
  
  // Close OpenGL window and terminate GLFW
  glfwTerminate();
  
  return 0;
}





//////////////////////////////////////////////////////////////////////
