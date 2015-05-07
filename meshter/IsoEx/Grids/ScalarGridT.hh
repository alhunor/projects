
//=============================================================================
//
//  CLASS ScalarGridT
//
//=============================================================================


#ifndef ISOEX_SCALARGRIDT_HH
#define ISOEX_SCALARGRIDT_HH


#include <OpenMesh/Core/IO/BinaryHelper.hh>
#include <OpenMesh/Core/Geometry/VectorT.hh>
#include <IsoEx/Grids/RegularGrid.hh>
#include <IsoEx/Implicits/Implicit.hh>
#include <vector>
#include <iostream>

namespace IsoEx {


//* \class ScalarGridT ScalarGridT.hh <IsoEx/Grids/ScalarGridT.hh>
    
///    This grid just holds a vector of scalar values.

template <typename Scalar>
class ScalarGridT : public RegularGrid
{
public:

  //typedef _Scalar Scalar;
  typedef std::vector<Scalar>  Values;


  /// Default constructor
  ScalarGridT(const OpenMesh::Vec3f&  _origin = OpenMesh::Vec3f(0,0,0),
	      const OpenMesh::Vec3f&  _x_axis = OpenMesh::Vec3f(1,0,0),
	      const OpenMesh::Vec3f&  _y_axis = OpenMesh::Vec3f(0,1,0),
	      const OpenMesh::Vec3f&  _z_axis = OpenMesh::Vec3f(0,0,1),
	      unsigned int            _x_res  = 10,
	      unsigned int            _y_res  = 10,
	      unsigned int            _z_res  = 10) 
    : RegularGrid(_origin, _x_axis, _y_axis, _z_axis, _x_res, _y_res, _z_res),
      values_(_x_res*_y_res*_z_res, 0)
  {}

  /// Destructor
  virtual ~ScalarGridT() {}



  virtual float scalar_distance(PointIdx _pidx) const
  {
    return values_[_pidx];
  }

  virtual bool is_inside(PointIdx _pidx) const
  {
    return values_[_pidx] < 0.0;
  }

  virtual bool directed_distance(const OpenMesh::Vec3f&  _p0,
				 const OpenMesh::Vec3f&  _p1,
				 OpenMesh::Vec3f&        _point,
				 OpenMesh::Vec3f&        _normal,
				 float&                  _distance) const {
    return false;
  }

  void sample(const Implicit& _implicit);


  virtual bool read(const char* _filename);
  virtual bool write(const char* _filename);
  virtual bool read(FILE* _in);
  virtual bool write(FILE* _out);


  Scalar& operator()(unsigned int x, unsigned int y, unsigned int z) {
    return values_[x + y*x_resolution() + z*x_resolution()*y_resolution()];
  }

  Scalar operator()(unsigned int x, unsigned int y, unsigned int z) const {
    return values_[x + y*x_resolution() + z*x_resolution()*y_resolution()];
  }

  Scalar& value(unsigned int x, unsigned int y, unsigned int z) {
    return (*this)(x, y, z);
  }

  Scalar value(unsigned int x, unsigned int y, unsigned int z) const {
    return (*this)(x, y, z);
  }

  void resize() {values_ = Values(n_points(), 0);};  // changed


private:
  
  Values  values_;
};

//  CLASS ScalarGridT - IMPLEMENTATION

//#define ISOEX_SCALARGRIDT_C

template <class Scalar>
void ScalarGridT<Scalar>::sample(const Implicit& _implicit)
{
  for (unsigned int i=0; i<n_points(); ++i)
    values_[i] = _implicit.scalar_distance(point(i));
}



template <class Scalar>
bool ScalarGridT<Scalar>::read(const char* _filename)
{
  bool ok = false;
  FILE* in = fopen(_filename, "rb");
  if (in)
  {
    ok = read(in);
    fclose(in);
  }
  return ok;
}


template <class Scalar>
bool ScalarGridT<Scalar>::read(FILE* _in)
{
  // header
  OpenMesh::Vec3f  origin, x_axis, y_axis, z_axis;
  unsigned int     x_res, y_res, z_res;

  
  fscanf(_in, 
	 "%f %f %f\n%f %f %f\n%f %f %f\n%f %f %f\n%d %d %d\n",
	 &origin[0], &origin[1], &origin[2], 
	 &x_axis[0], &x_axis[1], &x_axis[2], 
	 &y_axis[0], &y_axis[1], &y_axis[2], 
	 &z_axis[0], &z_axis[1], &z_axis[2], 
	 &x_res, &y_res, &z_res);

  initialize(origin, x_axis, y_axis, z_axis, x_res, y_res, z_res);
  

  // values
  values_ = Values(n_points(), 0);
  for (unsigned int i=0; i<n_points(); ++i)
    values_[i] = OpenMesh::IO::read_float(_in);


  return true;
}


//-----------------------------------------------------------------------------


template <class Scalar>
bool ScalarGridT<Scalar>::write(const char* _filename)
{
  bool ok = false;
  FILE* out = fopen(_filename, "wb");
  if (out)
  {
    ok = write(out);
    fclose(out);
  }
  return ok;
}


template <class Scalar>
bool ScalarGridT<Scalar>::write(FILE* _out)
{
  // header
  fprintf(_out,
	  "%f %f %f\n%f %f %f\n%f %f %f\n%f %f %f\n%d %d %d\n",
	  origin()[0], origin()[1], origin()[2], 
	  x_axis()[0], x_axis()[1], x_axis()[2], 
	  y_axis()[0], y_axis()[1], y_axis()[2], 
	  z_axis()[0], z_axis()[1], z_axis()[2], 
	  x_resolution(), y_resolution(), z_resolution());

  
  // values
  for (unsigned int i=0; i<n_points(); ++i)
    OpenMesh::IO::write_float(values_[i], _out);


  return true;
}

//=============================================================================
} // namespace IsoEx
//=============================================================================

//=============================================================================
#endif // ISOEX_SCALARGRIDT_HH defined
//=============================================================================

