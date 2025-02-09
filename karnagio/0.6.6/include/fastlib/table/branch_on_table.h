#ifndef FL_LITE_FASTLIB_TABLE_BRANCH_ON_TABLE_H_
#define FL_LITE_FASTLIB_TABLE_BRANCH_ON_TABLE_H_
//#include <omp.h>

namespace boost {
 namespace program_options {
   class variables_map;
 }
}
namespace fl { namespace table {
  class Branch {
    public:
      template<typename AlgorithmType, typename DataAccessType>
      static int BranchOnTable(DataAccessType *data,
          boost::program_options::variables_map &vm);
  };
}}

#endif
