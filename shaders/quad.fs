#version 410 core

out vec4 fcolor;

in vec3 TexCoord;

uniform sampler3D volumeTex;

//must be in projection space.
uniform vec3 cameraPosInSTR;

//Between 0.0 and 1.0
uniform float isovalue;
//Step advanced for each ray, keep this relatively small, gets costly
uniform float stepsize;

//Cube color
uniform vec3 background; 

int tfType = 0;
const float PI = 3.1415926535;

float mu = 0.5;
float sigma = 0.55;

vec4 TF(float scalar)
{
   if(tfType == 0)
      return vec4(scalar);
   
   if(tfType == 1)
   {
      float c = pow(scalar-mu, 2);
      float c2 = 2 * pow(sigma, 2);
      float num = exp(-c/c2); 
      float den = pow(2*PI, 0.5) * sigma;
      scalar = num/den;
      return vec4(scalar);
   }
}

void main()
{
   //background color
   vec4 dst = vec4(background, 0.0);
   //Iteration of loops, # of ray can maximum traverse (upper bound), iter > stepsize
   int iter = 500;

   vec3 rayDir = normalize(TexCoord - cameraPosInSTR);
   vec3 step = rayDir / stepsize;

   //This the intersection point in st/uv space 
   vec3 pos = TexCoord;

   vec4 src = vec4(0.0);
   //ray casting algorithm
   for(int i = 0; i < iter; i++)
   {
      float scalar = texture(volumeTex, pos).r;

      //if(scalar < isovalue)
         //src = dst;
      //else
         src = TF(scalar);
      // TODO
      // ^^^^^^^^, implement transfer function here, 
      // tf is just a function that map scalar to rgba value
   
      //back to front alpha blending src over dst
      dst = (1.0 - src.a) * dst + src.a * src;

      //if(dst.a >= .95f)
            //break; 
      //advance ray
      pos += step; 
      //check if pos out of bound
      if(pos.x > 1.0 || pos.y > 1.0 || pos.z > 1.0 
         || pos.x < 0.0 || pos.y < 0.0 || pos.z < 0.0)
         {
            break;
         }
   }

   //Output the color
   //gamma correction
   fcolor = pow(dst, vec4(1/2.2));
}