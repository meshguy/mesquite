#ifndef MSQ_PLANAR_DOMAIN_HPP
#define MSQ_PLANAR_DOMAIN_HPP

#include "MeshInterface.hpp"

namespace Mesquite
{
  class Mesh;
  
  class PlanarDomain : public Mesquite::MeshDomain
  {
  public:
    PlanarDomain(const Vector3D& normal,
                 const Vector3D& point,
                 Mesquite::Mesh* mesh)
        : mNormal(normal / normal.length()),
          mPoint(point),
          mMesh(mesh)
      {}
    
    void set_plane(const Vector3D& normal, const Vector3D& point)
      {
        mNormal = normal / normal.length();
        mPoint = point;
      }

    void set_mesh(Mesquite::Mesh* mesh)
      { mMesh = mesh; }
    
    virtual void snap_to(Mesh::EntityHandle entity_handle,
                         Vector3D &coordinate);
    
    virtual void normal_at(Mesh::EntityHandle entity_handle,
                           Vector3D &coordinate);

  private:
    Vector3D mNormal;
    Vector3D mPoint;
    Mesquite::Mesh* mMesh;
  };
}

#endif