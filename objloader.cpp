#include "objloader.h"
#include <fstream>
#include <sstream>
#include <ctime>

using namespace std;

ObjLoader::ObjLoader(void)
{
    srand((unsigned)time(0));
}

ObjLoader::~ObjLoader(void)
{
}

void ObjLoader::loadObj(string filename)
{

    float cur_count;
    float max_range = 0.f;

    obj_vertices.clear();
    obj_normals.clear();
    obj_texcoords.clear();
    obj_elements.clear();
    obj_phases.clear();

    ifstream inpfile(filename.c_str());
    if (!inpfile.is_open()) {
        cout << "Unable to open file" << endl;
        return;
    }
    string line;
    while(!getline(inpfile,line).eof()) {
        vector<string> splitline;
        string buf;

        stringstream ss(line);
        while (ss >> buf) {
            splitline.push_back(buf);
        }

        //Ignore blank lines
        if(splitline.size() == 0) {
            continue;
        }
        //Vertex
        if (splitline[0][0] == 'v') {
            if (splitline[0].length() > 1 && splitline[0][1] == 'n'){
                //normal, take care of this later
            } else if (splitline[0].length() > 1 && splitline[0][1] == 't'){
                //texture, take care of this later
            }
            else {
                glm::vec4 cur_v(atof(splitline[1].c_str()),
                    atof(splitline[2].c_str()),atof(splitline[3].c_str()), 0);
                obj_vertices.push_back(cur_v);
                obj_phases.push_back(cur_count += 10);
                max_range = max(max_range, abs(cur_v.x));
                max_range = max(max_range, abs(cur_v.y));
                max_range = max(max_range, abs(cur_v.z));
                //obj_phases.push_back((float)rand()/(float)RAND_MAX);
            }
        }
        //Face
        else if (splitline[0][0] == 'f') {
            int v1, v2, v3;
            int n1, n2, n3;
            int t1, t2, t3;
            //find "type"
            int num_slash = 0;
            bool double_slash = false;
            bool quad = false;
            if (splitline.size() == 5) {
                quad = true;
            }
            //std::cout << quad << std::endl;
            for (unsigned int i=0; i<splitline[1].length(); i++) {
                if (splitline[1][i] == '/')
                {
                    num_slash++;
                    if (i+1 < splitline[1].length() && splitline[1][i+1] == '/')
                    {
                        double_slash = true;
                    }
                }
            }
            //cout << numSlash << endl;
            if (num_slash == 0) {
                sscanf(line.c_str(), "f %d %d %d", &v1, &v2, &v3);
            } else if (num_slash == 1) {
                sscanf(line.c_str(), "f %d/%*d %d/%*d %d/%*d", &v1, &v2, &v3);
            } else if (num_slash == 2) {
                if (double_slash)
                {
                    sscanf(line.c_str(), "f %d//%d %d//%d %d//%d",
                        &v1, &n1, &v2, &n2, &v3, &n3);
                } else {
                    sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d",
                        &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);
                }
            } else {
                cout << "Too many slashses in f" << endl;
                continue;
            }
            obj_elements.push_back(v1-1);
            obj_elements.push_back(v2-1);
            obj_elements.push_back(v3-1);

        }
    }
    inpfile.close();

    num_vertices = obj_vertices.size();
    std::cout << "vertices: " << num_vertices << std::endl;
    std::cout << "vertices log10: " << std::log10(num_vertices) << std::endl;


    //rescale so the model is inside a unit cube
    for (std::vector<glm::vec4>::iterator it = obj_vertices.begin();
        it != obj_vertices.end(); ++it)
    {
        it->x = it->x / max_range;
        it->y = it->y / max_range;
        it->z = it->z / max_range;
    }

    //ignore textures for now, use geometric averaged normal
    vector<int> nb_seen;
    obj_normals.resize(obj_vertices.size(), glm::vec3(0.,0.,0.));
    nb_seen.resize(obj_vertices.size(), 0);
    for (int i = 0; i < obj_elements.size(); i += 3)
    {
        GLushort ia = obj_elements[i];
        GLushort ib = obj_elements[i+1];
        GLushort ic = obj_elements[i+2];

        glm::vec3 normal = glm::normalize(glm::cross(
            glm::vec3(obj_vertices[ib]) - glm::vec3(obj_vertices[ia]),
            glm::vec3(obj_vertices[ic]) - glm::vec3(obj_vertices[ia])));
        //normal = glm::vec3(0,1,0);

        int v[3];
        v[0] = ia; v[1] = ib; v[2] = ic;

        for (int j = 0; j < 3; j++)
        {
            GLushort cur_v = v[j];
            ++nb_seen[cur_v];
            if (nb_seen[cur_v] == 1)
            {
                obj_normals[cur_v] = normal;
            } else {
                obj_normals[cur_v].x = obj_normals[cur_v].x
                    * (1. - 1./nb_seen[cur_v]) + normal.x * 1./nb_seen[cur_v];
                obj_normals[cur_v].y = obj_normals[cur_v].y
                    * (1. - 1./nb_seen[cur_v]) + normal.y * 1./nb_seen[cur_v];
                obj_normals[cur_v].z = obj_normals[cur_v].z
                    * (1. - 1./nb_seen[cur_v]) + normal.z * 1./nb_seen[cur_v];
                obj_normals[cur_v] = glm::normalize(obj_normals[cur_v]);
            }
        }
    }
}

void ObjLoader::upload()
{
    if (obj_vertices.size() > 0)
    {
        glGenBuffers(1, &vbo_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
        glBufferData(GL_ARRAY_BUFFER,
            obj_vertices.size() * sizeof(obj_vertices[0]),
            obj_vertices.data(), GL_STATIC_DRAW);
    }

    if (obj_normals.size() > 0)
    {
        glGenBuffers(1, &vbo_normals);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
        glBufferData(GL_ARRAY_BUFFER,
            obj_normals.size() * sizeof(obj_normals[0]),
            obj_normals.data(), GL_STATIC_DRAW);
    }

    if (obj_elements.size() > 0)
    {
        glGenBuffers(1, &ibo_elements);
        glBindBuffer(GL_ARRAY_BUFFER, ibo_elements);
        glBufferData(GL_ARRAY_BUFFER,
            obj_elements.size() * sizeof(obj_elements[0]),
            obj_elements.data(), GL_STATIC_DRAW);
    }

    if (obj_phases.size() > 0)
    {
        glGenBuffers(1, &vbo_phases);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_phases);
        glBufferData(GL_ARRAY_BUFFER,
            obj_phases.size() * sizeof(obj_phases[0]),
            obj_phases.data(), GL_STATIC_DRAW);
    }

}

void ObjLoader::draw(Shader * shader)
{
    int v_coord_attrib = shader->attribute_v_coord;
    int v_normal_attrib = shader->attribute_v_normal;
    int v_phase_attrib = shader->attribute_v_phase;
    glEnableVertexAttribArray(v_coord_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glVertexAttribPointer(
        v_coord_attrib,
        4,
        GL_FLOAT,
        GL_FALSE,
        0,
        0
        );

    glEnableVertexAttribArray(v_normal_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
    glVertexAttribPointer(
        v_normal_attrib,
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        0
        );

    glEnableVertexAttribArray(v_phase_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_phases);
    glVertexAttribPointer(
        v_phase_attrib,
        1,
        GL_FLOAT,
        GL_FALSE,
        0,
        0
        );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
    int size;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    glDisableVertexAttribArray(v_coord_attrib);
    glDisableVertexAttribArray(v_normal_attrib);
}
