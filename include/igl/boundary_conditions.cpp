#include "boundary_conditions.h"

#include "verbose.h"
#include "EPS.h"
#include "project_to_line.h"

#include <vector>
#include <map>

IGL_INLINE bool igl::boundary_conditions(
  const Eigen::MatrixXd & V  ,
  const Eigen::MatrixXi & /*Ele*/,
  const Eigen::MatrixXd & C  ,
  const Eigen::VectorXi & P  ,
  const Eigen::MatrixXi & BE ,
  const Eigen::MatrixXi & CE ,
  Eigen::VectorXi &       b  ,
  Eigen::MatrixXd &       bc )
{
  using namespace Eigen;
  using namespace igl;
  using namespace std;

  if(P.size()+BE.rows() == 0)
  {
    verbose("^%s: Error: no handles found\n",__FUNCTION__);
    return false;
  }


  vector<int> bci;
  vector<int> bcj;
  vector<int> bcv;

  // loop over points
  for(int p = 0;p<P.size();p++)
  {
    VectorXd pos = C.row(P(p));
    // loop over domain vertices
    for(int i = 0;i<V.rows();i++)
    {
      // Find samples just on pos
      //Vec3 vi(V(i,0),V(i,1),V(i,2));
      double sqrd = (V.row(i)-pos).array().pow(2).sum();
      if(sqrd <= FLOAT_EPS)
      {
        bci.push_back(i);
        bcj.push_back(p);
        bcv.push_back(1.0);
      }
    }
  }

  // loop over bone edges
  for(int e = 0;e<BE.rows();e++)
  {
    // loop over domain vertices
    for(int i = 0;i<V.rows();i++)
    {
      // Find samples from tip up to tail
      VectorXd tip = C.row(BE(e,0));
      VectorXd tail = C.row(BE(e,1));
      // Compute parameter along bone and squared distance
      double t,sqrd;
      project_to_line(
          V(i,0),V(i,1),V(i,2),
          tip(0),tip(1),tip(2),
          tail(0),tail(1),tail(2),
          t,sqrd);
      if(t>=-FLOAT_EPS && t<=(1.0f+FLOAT_EPS) && sqrd<=FLOAT_EPS)
      {
        bci.push_back(i);
        bcj.push_back(P.size()+e);
        bcv.push_back(1.0);
      }
    }
  }

  // Cage edges are not considered yet
  // loop over cage edges
  for(int e = 0;e<CE.rows();e++)
  {
    // loop over domain vertices
    for(int i = 0;i<V.rows();i++)
    {
      // Find samples from tip up to tail
      VectorXd tip = C.row(P(CE(e,0)));
      VectorXd tail = C.row(P(CE(e,1)));
      // Compute parameter along bone and squared distance
      double t,sqrd;
      project_to_line(
          V(i,0),V(i,1),V(i,2),
          tip(0),tip(1),tip(2),
          tail(0),tail(1),tail(2),
          t,sqrd);
      if(t>=-FLOAT_EPS && t<=(1.0f+FLOAT_EPS) && sqrd<=FLOAT_EPS)
      {
        bci.push_back(i);
        bcj.push_back(CE(e,0));
        bcv.push_back(1.0-t);
        bci.push_back(i);
        bcj.push_back(CE(e,1));
        bcv.push_back(t);
      }
    }
  }

  // find unique boundary indices
  vector<int> vb = bci;
  sort(vb.begin(),vb.end());
  vb.erase(unique(vb.begin(), vb.end()), vb.end());

  b.resize(vb.size());
  bc = MatrixXd::Zero(vb.size(),P.size()+BE.rows());
  // Map from boundary index to index in boundary
  map<int,int> bim;
  int i = 0;
  // Also fill in b
  for(vector<int>::iterator bit = vb.begin();bit != vb.end();bit++)
  {
    b(i) = *bit;
    bim[*bit] = i;
    i++;
  }

  // Build BC
  for(i = 0;i < (int)bci.size();i++)
  {
    assert(bim.find(bci[i]) != bim.end());
    bc(bim[bci[i]],bcj[i]) = bcv[i];
  }

  // Normalize accross rows so that conditions sum to one
  for(i = 0;i<bc.rows();i++)
  {
    double sum = bc.row(i).sum();
    assert(sum != 0);
    bc.row(i).array() /= sum;
  }

  // Check that every Weight function has at least one boundary value of 1 and
  // one value of 0
  for(i = 0;i<bc.cols();i++)
  {
    double min_abs_c = bc.col(i).array().abs().minCoeff();
    double max_c = bc.col(i).maxCoeff();
    if(min_abs_c > FLOAT_EPS)
    {
      verbose("^%s: Error: handle %d does not receive 0 weight\n",__FUNCTION__,i);
      return false;
    }
    if(max_c< (1-FLOAT_EPS))
    {
      verbose("^%s: Error: handle %d does not receive 1 weight\n",__FUNCTION__,i);
      return false;
    }
  }

  return true;
}
