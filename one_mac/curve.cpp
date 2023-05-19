#ifdef __APPLE__
#include <OpenGL/gl.h>
/* Just in case we need these later
// References:
// http://alumni.cs.ucsb.edu/~wombatty/tutorials/opengl_mac_osx.html
// # include <OpenGL/gl.h>
// # include <OpenGL/glu.h>
*/
#else
#include <GL/gl.h>
#endif

#include "Matrix4f.h"
#include "curve.h"
#include "extra.h"
#ifdef WIN32
#include <windows.h>
#endif
using namespace std;

namespace
{
    // Approximately equal to.  We don't want to use == because of
    // precision issues with floating point.
    inline bool approx( const Vector3f& lhs, const Vector3f& rhs )
    {
        const float eps = 1e-8f;
        return ( lhs - rhs ).absSquared() < eps;
    }


}


Curve evalCurve( const std::vector< Vector3f >& P_triplet, unsigned steps, const Matrix4f& basis_vector )
{

    Curve return_val;
    steps = 100 * steps;
    // Check
    if( P_triplet.size() > 4 || P_triplet.size() % 3 != 1 )
    {
        cerr << "evalCurve must be called with 3 control points." << endl;
        exit( 0 );
    }

    // TODO:
    // You should implement this function so that it returns a Curve
    // (e.g., a vector< CurvePoint >).  The variable "steps" tells you
    // the number of points to generate on each piece of the spline.
    // At least, that's how the sample solution is implemented and how
    // the SWP files are written.  But you are free to interpret this
    // variable however you want, so long as you can control the
    // "resolution" of the discretized spline curve with it.

    // Bezier curve equation is as below :

    Matrix4f geometry_vector = Matrix4f(P_triplet[0].x() , P_triplet[1].x(), P_triplet[2].x(), P_triplet[3].x(),
                                    P_triplet[0].y() , P_triplet[1].y(), P_triplet[2].y(), P_triplet[3].y(),
                                    P_triplet[0].z() , P_triplet[1].z(), P_triplet[2].z(), P_triplet[3].z(),
                                    0       ,  0        , 0         ,   0);

    for(float i=0;i<steps;i++)
    {

        float coeff = (i/steps);
        float first_coeff = 1;
        float second_coeff = coeff;
        float third_coeff = coeff * coeff;
        float fourth_coeff = coeff * coeff * coeff;
        auto basis_poly = Matrix4f(first_coeff,       0, 0, 0,
                                   second_coeff,      0, 0, 0,
                                   third_coeff,       0, 0, 0,
                                   fourth_coeff,      0, 0, 0);



        auto mul_const = basis_vector * basis_poly;

        const auto gb_vec = geometry_vector * mul_const;


        Matrix4f result_vec = gb_vec * basis_poly;
        Vector4f vector4res = result_vec.getCol(0);
        Vector3f point_data(vector4res.x(),vector4res.y(),vector4res.z()); // Vertex
        CurvePoint curve_point;
        curve_point.V = point_data;
        return_val.push_back(curve_point);
    }

    // calculate tangents
    for(int i=0;i<steps;i++)
    {
        if(i == 0)
        {
            Vector3f point_data(0.0f,0.0f,0.0f); // Tangent
            return_val[i].T = point_data;
        }
        else
        {
            Vector3f deltaVec = (return_val[i].V - return_val[i-1].V);
            deltaVec.normalize();
            return_val[i].T = deltaVec;
        }
    }

    // calculate both normals
    for(int i=0;i<steps;i++)
    {
        if(i == 0)
        {
            Vector3f point_data_bi(0.0f,0.0f,1.0f); // bi-Normal
            Vector3f point_data_no(0.0f,0.0f,0.0f); // Normal
            return_val[i].B = point_data_bi;
            return_val[i].N = point_data_no;
        }
        else
        {
            Vector3f norm_vec = return_val[i-1].B.cross(return_val[i-1].B,return_val[i].T);
            norm_vec.normalize();
            return_val[i].N = norm_vec;
            Vector3f binorm_vec = return_val[i].N.cross(return_val[i].N,return_val[i].T);
            binorm_vec.normalize();
            return_val[i].B = binorm_vec;
        }
    }



    // Make sure that this function computes all the appropriate
    // Vector3fs for each CurvePoint: V,T,N,B.
    // [NBT] should be unit and orthogonal.

    // Also note that you may assume that all Bezier curves that you
    // receive have G1 continuity.  Otherwise, the TNB will not be
    // be defined at points where this does not hold.

    cerr << "\t>>> evalCurve has been called with the following input:" << endl;

    cerr << "\t>>> Control points (type vector< Vector3f >): "<< endl;
    for( unsigned i = 0; i < P_triplet.size(); ++i )
    {
        cerr << "\t>>> " << P_triplet[i].x() << "," << P_triplet[i].y() << "," << P_triplet[i].z()  << endl;
    }

    return return_val;

}

const Matrix4f bezier_basis_matrix =  Matrix4f(1,-3,3,-1,0,3,-6,3,0,0,3,-3,0,0,0,1);

Curve evalBezier( const vector< Vector3f >& P, unsigned steps )
{
    Curve return_val;
    cerr << "\t>>> evalBezier is triggered ! " << endl;

    for(int i=0;i<P.size();i=i+4)
    {
        vector< Vector3f > P_vec;

        P_vec.push_back(P[i]);
        P_vec.push_back(P[i+1]);
        P_vec.push_back(P[i+2]);
        P_vec.push_back(P[i+3]);
        Curve local_curve = evalCurve(P_vec,steps,bezier_basis_matrix);
        for(int i=0;i<local_curve.size();i++)
        {
            return_val.push_back(local_curve[i]);
        }
    }
    return return_val;
}

const Matrix4f bspline_basis_matrix =  Matrix4f(0.1667f,-0.5f,0.5f,-0.1667f,0.6667,0.0f,-1.0f,0.5f,0.1667f,0.5f,0.5f,-0.5f,0,0,0,0.1667f);

Curve evalBspline( const vector< Vector3f >& P, unsigned steps )
{
    Curve return_val;
    Matrix4f bspline_basis_matrix_adjusted = bspline_basis_matrix;

    cerr << "\t>>> evalBspline is triggered ! " << endl;

    bspline_basis_matrix_adjusted.print();
    for(int i=0;i<(P.size()-3);i=i+1)
    {
        vector< Vector3f > P_vec;

        P_vec.push_back(P[i]);
        P_vec.push_back(P[i+1]);
        P_vec.push_back(P[i+2]);
        P_vec.push_back(P[i+3]);
        Curve local_curve = evalCurve(P_vec,steps,bspline_basis_matrix_adjusted);
        int end_limit = 0;
        if(i == (P.size()-4))
        {
            end_limit = local_curve.size();
        }
        else
        {
            end_limit = (local_curve.size() / 4);
        }
        for(int i=0;i<end_limit;i++)
        {
            return_val.push_back(local_curve[i]);
        }
    }
    return return_val;
}

Curve evalCircle( float radius, unsigned steps )
{
    // This is a sample function on how to properly initialize a Curve
    // (which is a vector< CurvePoint >).

    // Preallocate a curve with steps+1 CurvePoints
    Curve R( steps+1 );

    // Fill it in counterclockwise
    for( unsigned i = 0; i <= steps; ++i )
    {
        // step from 0 to 2pi
        float t = 2.0f * M_PI * float( i ) / steps;

        // Initialize position
        // We're pivoting counterclockwise around the y-axis
        R[i].V = radius * Vector3f( cos(t), sin(t), 0 );

        // Tangent vector is first derivative
        R[i].T = Vector3f( -sin(t), cos(t), 0 );

        // Normal vector is second derivative
        R[i].N = Vector3f( -cos(t), -sin(t), 0 );

        // Finally, binormal is facing up.
        R[i].B = Vector3f( 0, 0, 1 );
    }

    return R;
}

void drawCurve( const Curve& curve, float framesize )
{
    // Save current state of OpenGL
    glPushAttrib( GL_ALL_ATTRIB_BITS );

    // Setup for line drawing
    glDisable( GL_LIGHTING );
    glColor4f( 1, 1, 1, 1 );
    glLineWidth( 1 );

    // Draw curve
    glBegin( GL_LINE_STRIP );
    for( unsigned i = 0; i < curve.size(); ++i )
    {
        glVertex( curve[ i ].V );
    }
    glEnd();

    glLineWidth( 1 );

    // Draw coordinate frames if framesize nonzero
    if( framesize != 0.0f )
    {
        Matrix4f M;

        for( unsigned i = 0; i < curve.size(); ++i )
        {
            M.setCol( 0, Vector4f( curve[i].N, 0 ) );
            M.setCol( 1, Vector4f( curve[i].B, 0 ) );
            M.setCol( 2, Vector4f( curve[i].T, 0 ) );
            M.setCol( 3, Vector4f( curve[i].V, 1 ) );

            glPushMatrix();
            glMultMatrixf( M );
            glScaled( framesize, framesize, framesize );
            glBegin( GL_LINES );
            glColor3f( 1, 0, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 1, 0, 0 );
            glColor3f( 0, 1, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 1, 0 );
            glColor3f( 0, 0, 1 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 0, 1 );
            glEnd();
            glPopMatrix();
        }
    }

    // Pop state
    glPopAttrib();
}
