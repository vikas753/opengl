#include "surf.h"
#include "extra.h"
#include <cmath>
using namespace std;

namespace
{

    // We're only implenting swept surfaces where the profile curve is
    // flat on the xy-plane.  This is a check function.
    static bool checkFlat(const Curve &profile)
    {
        for (unsigned i=0; i<profile.size(); i++)
            if (profile[i].V[2] != 0.0 ||
                profile[i].T[2] != 0.0 ||
                profile[i].N[2] != 0.0)
                return false;

        return true;
    }
}

Surface makeSurfRev(const Curve &profile, unsigned steps)
{
    Surface surface;
    float steps_f = (float)steps;
    int curve_size = profile.size();

    if (!checkFlat(profile))
    {
        cerr << "surfRev profile curve must be flat on xy plane." << endl;
        exit(0);
    }
    const float factor_step_const = (2 * M_PI) / steps_f;
    unsigned unadjusted_surface_vec_index = 0;
    unsigned max_index = curve_size * steps_f;
    //cout << " Factor Step : " << factor_step_const << " Last Index : " << (2 * M_PI) << " profile size : " << profile.size() << endl;

    for(int i=0;i<curve_size;i++)
    {
        for(float factor_step = (2 * M_PI) / steps_f ; factor_step < (2 * M_PI) ; (factor_step = factor_step + factor_step_const))
        {

            Matrix3f rotation_matrix(0.0f);
            rotation_matrix = rotation_matrix.rotateY(factor_step);
            Matrix3f vector_data_vertex = Matrix3f(0.0f);
            vector_data_vertex.setCol(0, profile[i].V);


            Matrix3f vector_data_normal = Matrix3f(0.0f);
            vector_data_normal.setCol(0, profile[i].N);


            Matrix3f result_data_vertex = rotation_matrix *  vector_data_vertex;
            //cout << " x : " << profile[i].V.x() << " y : " << profile[i].V.y() << " z : " << profile[i].V.z() << endl;
            //rotation_matrix.print();

            surface.VV.push_back(result_data_vertex.getCol(0));

            //cout << " res x : " << result_data_vertex.getCol(0).x() << " y : " << result_data_vertex.getCol(0).y() << " z : " << result_data_vertex.getCol(0).z() << endl;

            Matrix3f result_data_normal = rotation_matrix *  vector_data_normal;
            surface.VN.push_back(result_data_normal.getCol(0));

            unsigned next_diag_index = unadjusted_surface_vec_index + steps;

            Tup3u triangle_0(unadjusted_surface_vec_index , (next_diag_index+1), next_diag_index);
            Tup3u triangle_1(unadjusted_surface_vec_index , (unadjusted_surface_vec_index+1), (next_diag_index+1));

            if(next_diag_index < max_index)
            {
                surface.VF.push_back(triangle_0);
                surface.VF.push_back(triangle_1);
            }

            unadjusted_surface_vec_index = unadjusted_surface_vec_index + 1;

        }
    }
    //cout << " surface size : " << surface.VV.size() << endl;

    return surface;
}

Surface makeGenCyl(const Curve &profile, const Curve &sweep )
{
    Surface surface;

    if (!checkFlat(profile))
    {
        cerr << "genCyl profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    unsigned profile_curve_size = profile.size();
    unsigned curve_size = sweep.size();
    unsigned unadjusted_surface_vec_index = 0;
    unsigned max_index = curve_size * profile_curve_size;


    for(int i=0;i<curve_size;i++)
    {
        Matrix4f transform_matrix = Matrix4f(sweep[i].N.x() , sweep[i].B.x() , sweep[i].T.x() , sweep[i].V.x(),
                                             sweep[i].N.y() , sweep[i].B.y() , sweep[i].T.y() , sweep[i].V.y(),
                                             sweep[i].N.z() , sweep[i].B.z() , sweep[i].T.z() , sweep[i].V.z(),
                                             0.0f           , 0.0f           , 0.0f           , 1.0f);

        Matrix4f inverse_transform_matrix = transform_matrix.inverse();
        inverse_transform_matrix.transpose();

        for(int j=0;j < profile_curve_size;j++)
        {

            Matrix4f basis_matrix = Matrix4f(1.0f , 1.0f , 1.0f , 0.0f,
                                             0.0f , 0.0f , 0.0f , 0.0f,
                                             0.0f , 0.0f , 0.0f , 0.0f,
                                             0.0f , 0.0f , 0.0f , 0.0f);

            Matrix4f profile_coord = Matrix4f(profile[j].V.x(), 0.0f, 0.0f, 0.0f,
                                              profile[j].V.y(), 0.0f, 0.0f, 0.0f,
                                              profile[j].V.z(), 0.0f, 0.0f, 0.0f,
                                              1.0f            , 0.0f, 0.0f, 0.0f);

            Matrix4f profile_norm  = Matrix4f(profile[j].N.x(), 0.0f, 0.0f, 0.0f,
                                              profile[j].N.y(), 0.0f, 0.0f, 0.0f,
                                              profile[j].N.z(), 0.0f, 0.0f, 0.0f,
                                              0.0f            , 0.0f, 0.0f, 0.0f);




            Matrix4f transform_coord = transform_matrix * profile_coord;
            Matrix4f norm_transform  = inverse_transform_matrix * profile_norm;

            Vector4f res_vec4d = transform_coord.getCol(0);
            Vector4f res_norm_vec4d = norm_transform.getCol(0);

            Vector3f res_vec3d(res_vec4d.x(),res_vec4d.y(),res_vec4d.z());
            Vector3f res_norm_vec3d(res_norm_vec4d.x(),res_norm_vec4d.y(),res_norm_vec4d.z());

            surface.VV.push_back(res_vec3d);
            surface.VN.push_back(res_norm_vec3d);

            unsigned next_diag_index = unadjusted_surface_vec_index + profile_curve_size;

            Tup3u triangle_0(unadjusted_surface_vec_index , (next_diag_index+1), next_diag_index);
            Tup3u triangle_1(unadjusted_surface_vec_index , (unadjusted_surface_vec_index+1), (next_diag_index+1));

            if(next_diag_index < max_index)
            {
                surface.VF.push_back(triangle_0);
                surface.VF.push_back(triangle_1);
            }

            unadjusted_surface_vec_index = unadjusted_surface_vec_index + 1;
        }
    }

    cout << " Surface Size : " << surface.VF.size() << endl;
    return surface;
}

void drawSurface(const Surface &surface, bool shaded)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    if (shaded)
    {
        // This will use the current material color and light
        // positions.  Just set these in drawScene();
        glEnable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // This tells openGL to *not* draw backwards-facing triangles.
        // This is more efficient, and in addition it will help you
        // make sure that your triangles are drawn in the right order.
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
    else
    {
        glDisable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glColor4f(0.4f,0.4f,0.4f,1.f);
        glLineWidth(1);
    }

    glBegin(GL_TRIANGLES);
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        glNormal(surface.VN[surface.VF[i][0]]);
        glVertex(surface.VV[surface.VF[i][0]]);
        glNormal(surface.VN[surface.VF[i][1]]);
        glVertex(surface.VV[surface.VF[i][1]]);
        glNormal(surface.VN[surface.VF[i][2]]);
        glVertex(surface.VV[surface.VF[i][2]]);
    }
    glEnd();

    glPopAttrib();
}

void drawNormals(const Surface &surface, float len)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glDisable(GL_LIGHTING);
    glColor4f(0,1,1,1);
    glLineWidth(1);

    glBegin(GL_LINES);
    for (unsigned i=0; i<surface.VV.size(); i++)
    {
        glVertex(surface.VV[i]);
        glVertex(surface.VV[i] + surface.VN[i] * len);
    }
    glEnd();

    glPopAttrib();
}

void outputObjFile(ostream &out, const Surface &surface)
{

    for (unsigned i=0; i<surface.VV.size(); i++)
        out << "v  "
            << surface.VV[i][0] << " "
            << surface.VV[i][1] << " "
            << surface.VV[i][2] << endl;

    for (unsigned i=0; i<surface.VN.size(); i++)
        out << "vn "
            << surface.VN[i][0] << " "
            << surface.VN[i][1] << " "
            << surface.VN[i][2] << endl;

    out << "vt  0 0 0" << endl;

    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        out << "f  ";
        for (unsigned j=0; j<3; j++)
        {
            unsigned a = surface.VF[i][j]+1;
            out << a << "/" << "1" << "/" << a << " ";
        }
        out << endl;
    }
}
