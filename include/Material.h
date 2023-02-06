#ifndef MATERIAL_H_INCLUDED
#define MATERIAL_H_INCLUDED

#include <glm/glm.hpp>


namespace be
{

    struct Material
    {
        glm::vec3  ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float shininess;
 
    protected:
        Material() = default;
    };


    struct Emerald : Material
    {
        Emerald()
        {
            ambient = { 0.0215	, 0.1745, 0.0215 };
            diffuse = { 0.07568,	0.61424, 0.07568 };
            specular = { 0.633, 0.727811 ,0.633 };
            shininess = 0.6f;
        }
    };

    struct Jade : Material
    {
        Jade()
        {
            ambient = { 0.135,	0.2225,	0.1575 };
            diffuse = { 0.54,	0.89,	0.63 };
            specular = { 0.316228,	0.316228,	0.316228 };
            shininess = 0.1;
        }
    };

    struct Obsidian : Material
    {
        Obsidian()
        {
            ambient = { 0.05375,	0.05,	0.06625 };
            diffuse = { 0.18275,	0.17,	0.22525 };
            specular = { 0.332741,	0.328634,	0.346435 };
            shininess = 0.3;
        }
    };

    struct Pearl : Material
    {
        Pearl()
        {
            ambient = { 0.25,	0.20725,	0.20725 };
            diffuse = { 1,	0.829,	0.829 };
            specular = { 0.296648,	0.296648,	0.296648 };
            shininess = 0.088;
        }
    };

    struct Ruby : Material
    {
        Ruby()
        {
            ambient = { 0.1745,	0.01175,	0.01175 };
            diffuse = { 0.61424,	0.04136,	0.04136 };
            specular = { 0.727811,	0.626959,	0.626959 };
            shininess = 0.6;
        }
    };

    struct Turquoise : Material
    {
        Turquoise()
        {
            ambient = { 0.1,	0.18725,	0.1745 };
            diffuse = { 0.396,	0.74151,	0.69102 };
            specular = { 0.297254,	0.30829,	0.306678 };
            shininess = 0.1;
        }
    };

    struct Brass : Material
    {
        Brass()
        {
            ambient = { 0.329412,	0.223529,	0.027451 };
            diffuse = { 0.780392,	0.568627,	0.113725 };
            specular = { 0.992157,	0.941176,	0.807843 };
            shininess = 0.21794872;
        }
    };

    struct Bronze : Material
    {
        Bronze()
        {
            ambient = { 0.2125,	0.1275,	0.054 };
            diffuse = { 0.714,	0.4284,	0.18144 };
            specular = { 0.393548,	0.271906,	0.166721 };
            shininess = 0.2;
        }
    };

    struct Chrome : Material
    {
        Chrome()
        {
            ambient = { 0.25,	0.25,	0.25 };
            diffuse = { 0.4,	0.4,	0.4 };
            specular = { 0.774597,	0.774597,	0.774597 };
            shininess = 0.6;
        }
    };


    struct Copper : Material
    {

        Copper()
        {
            ambient = { 0.19125,	0.0735,	0.0225 };
            diffuse = { 0.7038,	0.27048,	0.0828 };
            specular = { 0.256777,	0.137622,	0.086014 };
            shininess = 0.1;
        }
    };

    struct Gold : Material
    {
        Gold()
        {
            ambient = { 0.24725,	0.1995,	0.0745 };
            diffuse = { 0.75164,	0.60648,	0.22648 };
            specular = { 0.628281,	0.555802,	0.366065 };
            shininess = 0.4;
        }
    };

    struct Silver : Material
    {
        Silver()
        {
            ambient = { 0.19225,	0.19225,	0.19225 };
            diffuse = { 0.50754,	0.50754,	0.50754 };
            specular = { 0.508273,	0.508273,	0.508273 };
            shininess = 0.4;
        }
    };

    struct Black_Plastic : Material
    {
        Black_Plastic()
        {
            ambient = { 0.0,	0.0,	0.0 };
            diffuse = { 0.01,	0.01,	0.01 };
            specular = { 0.50,	0.50,	0.50 };
            shininess = 0.25;
        }
    };

    struct Cyan_Plastic : Material
    {
        Cyan_Plastic()
        {
            ambient = { 0.0,	0.1,	0.06 };
            diffuse = { 0.0,	0.50980392,	0.50980392 };
            specular = { 0.50196078,	0.50196078,	0.50196078 };
            shininess = 0.25;
        }
    };

    struct Green_Plastic : Material
    {
        Green_Plastic()
        {
            ambient = { 0.0,	0.0,	0.0 };
            diffuse = { 0.1,	0.35,	0.1 };
            specular = { 0.45,	0.55,	0.45 };
            shininess = 0.25;
        }
    };

    struct Red_Plastic : Material
    {
        Red_Plastic()
        {
            ambient = { 0.0,	0.0,	0.0 };
            diffuse = { 0.5,	0.0,	0.0 };
            specular = { 0.7,	0.6,	0.6 };
            shininess = 0.25;
        }
    };

    struct White_Plastic : Material
    {
        White_Plastic()
        {
            ambient = { 0.0,	0.0,	0.0 };
            diffuse = { 0.55,	0.55,	0.55 };
            specular = { 0.70,	0.70,	0.70 };
            shininess = 0.25;
        }
    };

    struct Yellow_Plastic : Material
    {
        Yellow_Plastic()
        {
            ambient = { 0.0,	0.0,	0.0 };
            diffuse = { 0.5,	0.5,	0.0 };
            specular = { 0.60,	0.60,	0.50 };
            shininess = 0.25;
        }
    };

    struct Black_Rubber : Material
    {
        Black_Rubber()
        {
            ambient = { 0.02,	0.02,	0.02 };
            diffuse = { 0.01,	0.01,	0.01 };
            specular = { 0.4,	0.4,	0.4 };
            shininess = 0.78125;
        }
    };

    struct Cyan_Rubber : Material
    {
        Cyan_Rubber()
        {
            ambient = { 0.0,	0.05,	0.05 };
            diffuse = { 0.4,	0.5,	0.5 };
            specular = { 0.04,	0.7,	0.7 };
            shininess = 0.78125;
        }
    };

    struct Green_Rubber : Material
    {
        Green_Rubber()
        {
            ambient = { 0.0,	0.05,	0.0 };
            diffuse = { 0.4,	0.5,	0.4 };
            specular = { 0.04,	0.7,	0.04 };
            shininess = 0.78125;
        }
    };

    struct Red_Rubber : Material
    {
        Red_Rubber()
        {
            ambient = { 0.05,	0.0,	0.0 };
            diffuse = { 0.5,	0.4,	0.4 };
            specular = { 0.7,	0.04,	0.04 };
            shininess = 0.78125;
        }
    };

    struct White_Rubber : Material
    {
        White_Rubber()
        {
            ambient = { 0.05,	0.05,	0.05 };
            diffuse = { 0.5,	0.5,	0.5 };
            specular = { 0.7,	0.7,	0.7 };
            shininess = 0.78125;
        }
    };

    struct Yellow_Rubber : Material
    {
        Yellow_Rubber()
        {
            ambient = { 0.05,	0.05,	0.0 };
            diffuse = { 0.5,	0.5,	0.4 };
            specular = { 0.7,	0.7,	0.04 };
            shininess = 0.78125;
        }
    };

}
#endif // MATERIAL_H_INCLUDED
