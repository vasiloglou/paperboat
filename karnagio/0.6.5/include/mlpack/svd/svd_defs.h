/*
Copyright © 2010, Ismion Inc
All rights reserved.
http://www.ismion.com/

Redistribution and use in source and binary forms, with or without
modification IS NOT permitted without specific prior written
permission. Further, neither the name of the company, Ismion
Inc, nor the names of its employees may be used to endorse or promote
products derived from this software without specific prior written
permission.

THIS SOFTWARE IS PROVIDED BY THE Ismion Inc "AS IS" AND ANY
EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COMPANY BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/
/**
 * @file svd_defs.cc
 *
 * This file implements command line interface for the QUIC-SVD
 * method. It approximate the original matrix by another matrix
 * with smaller dimension to a certain accuracy degree specified by the
 * user and then make SVD decomposition in the projected supspace.
 *
 * Run with --help for more usage.
 *
 * @see svd.h
 */

#ifndef FL_LITE_MLPACK_SVD_SVD_DEFS_H
#define FL_LITE_MLPACK_SVD_SVD_DEFS_H

#include <string>
#include "fastlib/util/string_utils.h"
#include "boost/program_options.hpp"
#include "fastlib/dense/matrix.h"
#include "fastlib/data/multi_dataset.h"
#include "svd.h"
#include "fastlib/workspace/task.h"
#include "fastlib/table/linear_algebra.h"
template<class WorkSpaceType, typename BranchType>
int fl::ml::Svd<boost::mpl::void_>::Main(WorkSpaceType *ws, const std::vector<std::string> &args) {

  FL_SCOPED_LOG(Svd);
  ////////// READING PARAMETERS AND LOADING DATA /////////////////////
  boost::program_options::options_description desc("Available options");
  desc.add_options()
  ("help", "Display help on SVD")
  ("references_in", boost::program_options::value<std::string>(),
   "File containing the dataset to SVD-decompose")
  ("algorithm", boost::program_options::value<std::string>()->default_value("covariance"),
   "covariance : LAPACK implementation on the covariance matrix\n"
   "sgdl       : compute low rank factorization with stochastic gradient descent\n"
   "             only the right factor will be orthogonal. \n"
   "sgdr       : compute low rank factorization with stochastic gradient descent\n"
   "             only the left factor will be orthogonal. \n"
   "lbfgs      : same as before but with lbfgs\n"
   "randomized : use a projection to a gaussian random matrix and then do svd\n"
   "concept    : uses the concept decomposition from Dhillon \"Concept Decompositions "
   "             for Large Sparse Text Data using Clustering\""
  )
  ("step0", 
   boost::program_options::value<double>()->default_value(1.0),
   "This is the step0 that sgdl, sgdr, the stochastic gradient decent"
  )
  ("randomize",
   boost::program_options::value<bool>()->default_value(true),
   "The stochastic gradient descent requires the data to be randomized. "
   "If you think that your data are not randomized then set this flag to true. "
   "It might slow down the performance but it will improve the results."
  )
  ("l2normalize", 
   boost::program_options::value<bool>()->default_value(true),
   "For svd through concept vector decomposition you need to have your data "
   "L2 mormalized. If your data is not L2 normalized then set this flag true"
  )("col_mean_normalize",
    boost::program_options::value<bool>()->default_value(false),
    "if you set this flag to true it will normalize the columns so that "
    "they have zero mean. In that case SVD is equivalent to PCA"
  )("n_epochs",
   boost::program_options::value<index_t>()->default_value(5),
   "number of epochs of stochastic gradient descent. "
   " each epoch finishes after n_iterations are completed"
  )
  ("n_iterations",
   boost::program_options::value<index_t>()->default_value(10),
   "number of iterations. Each iteration finishes after a pass over all "
   "reference table data."
  )
  ("rec_error", 
   boost::program_options::value<bool>()->default_value(false),
   "If you set this flag true then the reconstruction error will be computed"   
  )
  ("svd_rank", boost::program_options::value<int>()->default_value(5),
   "The algorithm will find up to the svd_rank first components")
  ("smoothing_p", boost::program_options::value<int>()->default_value(2),
   "when doing randomized svd you need to smooth the matrix by "
   "mutliplying it with XX' p times")
  ("lsv_out",
   boost::program_options::value<std::string>(),
   "The output file for the left singular vectors (each column is a singular vector).")
  ("sv_out",
   boost::program_options::value<std::string>(),
   "The output file for the singular values.")
  ("rsv_out",
   boost::program_options::value<std::string>(),
   "The output file for the transposed right singular vectors (each row is a singular vector).");


  boost::program_options::variables_map vm;
  boost::program_options::command_line_parser clp(args);
  clp.style(boost::program_options::command_line_style::default_style
            ^ boost::program_options::command_line_style::allow_guessing);
  try {
    boost::program_options::store(clp.options(desc).run(), vm);
  }
  catch(const boost::program_options::invalid_option_value &e) {
	  fl::logger->Die() << "Invalid Argument: " << e.what();
  }
  catch(const boost::program_options::invalid_command_line_syntax &e) {
	  fl::logger->Die() << "Invalid command line syntax: " << e.what(); 
  }
  catch (const boost::program_options::unknown_option &e) {
     fl::logger->Die() << e.what()
      <<" . This option will be ignored";
  }
  catch ( const boost::program_options::error &e) {
    fl::logger->Die() << e.what();
  } 
  boost::program_options::notify(vm);
  if (vm.count("help")) {
    fl::logger->Message() << fl::DISCLAIMER << "\n";
    fl::logger->Message() << desc << "\n";
    return true;
  }


  return BranchType::template BranchOnTable < Svd<boost::mpl::void_>,
         WorkSpaceType > (ws, vm);

}

template<typename TableType1>
template<typename WorkSpaceType>
int fl::ml::Svd<boost::mpl::void_>::Core<TableType1>::Main(
  WorkSpaceType *ws,
  boost::program_options::variables_map &vm) {

  typedef TableType1 Table_t;
  // The reference data file is a required parameter.
  if (!vm.count("references_in")) {
    fl::logger->Die() << "Option --references_in is required";
  }
  std::string references_file_name = vm["references_in"].as<std::string>();
  fl::logger->Message() << "Loading data from " << references_file_name << std::endl;
  boost::shared_ptr<TableType1> references_table; 
  ws->Attach(references_file_name, &references_table);
  if (vm["col_mean_normalize"].as<bool>()==true) {
    fl::logger->Message()<<"Performing zero mean normalization over the columns"
      <<std::endl;
    boost::shared_ptr<TableType1> new_references_table(new TableType1);
    new_references_table->Init("",
        references_table->dense_sizes(),
        references_table->sparse_sizes(),
        0);
    std::vector<double> means, variances;
    references_table->AttributeStatistics(&means, &variances);
    typename TableType1::Point_t point, point1;
    for(index_t i=0; i<references_table->n_entries(); ++i) {
      references_table->get(i, &point);
      point1.Copy(point);
      for(size_t j=0; j<means.size(); ++j) {
        point1.set(j, point[j]-means[j]);
      }
      new_references_table->push_back(point1);
    }
    references_table=new_references_table;
  }
  fl::logger->Message() << "Loading completed" << std::endl;

  boost::shared_ptr<typename WorkSpaceType::MatrixTable_t> left_table;
  boost::shared_ptr<typename WorkSpaceType::MatrixTable_t> right_trans_table;
  boost::shared_ptr<typename WorkSpaceType::MatrixTable_t> sv_table;

  std::string lsv_file;
  if (vm.count("lsv_out")) {
    lsv_file = vm["lsv_out"].as<std::string>();
  } else {
    lsv_file=ws->GiveTempVarName();
  }
  std::string sv_file;
  if (vm.count("sv_out")) {
    sv_file=vm["sv_out"].as<std::string>();
  } else {
    sv_file=ws->GiveTempVarName();
  }
  std::string rsv_trans_file;
  if (vm.count("rsv_out")) { 
    rsv_trans_file=vm["rsv_out"].as<std::string>();
  } else {
    rsv_trans_file=ws->GiveTempVarName(); 
  }

  int svd_rank = vm["svd_rank"].as<int>();
  if (svd_rank>references_table->n_attributes()) {
    fl::logger->Die()<<"--svd_rank ("<< svd_rank <<")must be less "
      "or equal to the --references_in attributes ("
      <<references_table->n_attributes()<<")";
  }

  ws->Attach(lsv_file, 
      std::vector<index_t>(1,svd_rank),
      std::vector<index_t>(),
      references_table->n_entries(), 
      &left_table);

  ws->Attach(rsv_trans_file, 
      std::vector<index_t>(1,svd_rank),
      std::vector<index_t>(),
      references_table->n_attributes(), 
      &right_trans_table);

  ws->Attach(sv_file, 
      std::vector<index_t>(1,1),
      std::vector<index_t>(),
      svd_rank, 
      &sv_table);

  Svd<TableType1> engine;
  if (vm["algorithm"].as<std::string>() == "covariance") {
    fl::logger->Message()<<"Computing SVD with LAPACK "
      "on the covariance matrix"<<std::endl;
    engine.ComputeFull(*references_table,
                svd_rank,
                sv_table.get(),
                left_table.get(),
                right_trans_table.get());
    fl::logger->Message() << "Finished computing SVD" << std::endl;
  } else {
    if (StringStartsWith(vm["algorithm"].as<std::string>(), "sgd")) {
      double step0=vm["step0"].as<double>();
      index_t n_epochs=vm["n_epochs"].as<index_t>();
      index_t n_iterations=vm["n_iterations"].as<index_t>();
      bool randomize=vm["randomize"].as<bool>();
      typename WorkSpaceType::MatrixTable_t temp_left,
               temp_right_trans;
      temp_left.Init("",
          std::vector<index_t>(1, svd_rank),
          std::vector<index_t>(),
          references_table->n_entries());
      temp_right_trans.Init("",
          std::vector<index_t>(1, svd_rank),
          std::vector<index_t>(),
          references_table->n_attributes());

      engine.ComputeLowRankSgd(*references_table,
                               step0,
                               n_epochs,
                               n_iterations,
                               randomize,
                               &temp_left,
                               &temp_right_trans); 
      if (vm["algorithm"].as<std::string>()=="sgdl") {
        typename WorkSpaceType::MatrixTable_t temp_left1;
        temp_left1.Init("",
            std::vector<index_t>(1, temp_right_trans.n_attributes()),
            std::vector<index_t>(),
            temp_right_trans.n_attributes());
        engine.ComputeFull(temp_right_trans,
                   svd_rank,
                   sv_table.get(),
                   right_trans_table.get(),
                   &temp_left1); 
        fl::table::Mul<fl::la::NoTrans, 
          fl::la::NoTrans>(temp_left, temp_left1, left_table.get());
        std::cout<<temp_left.n_entries()<<" x "<< temp_left.n_attributes()<<std::endl;
        std::cout<<temp_left1.n_entries()<<" x "<< temp_left1.n_attributes()<<std::endl;
      } else {
        if (vm["algorithm"].as<std::string>()=="sgdr") {
          typename WorkSpaceType::MatrixTable_t temp_right_trans1;
          temp_right_trans1.Init("",
            std::vector<index_t>(1, svd_rank),
            std::vector<index_t>(),
            svd_rank);

          engine.ComputeFull(temp_left,
                   svd_rank,
                   sv_table.get(),
                   left_table.get(),
                   &temp_right_trans1); 

          fl::table::Mul<fl::la::NoTrans, fl::la::NoTrans>(
              temp_right_trans,
              temp_right_trans1, 
              right_trans_table.get());
        } else {
          fl::logger->Die()<<"This option "
            <<vm["algorithm"].as<std::string>() 
            <<" is not supported";
        }
      }    
    } else {
      if (vm["algorithm"].as<std::string>() == "lbfgs") {
        fl::logger->Die()<<"Not supported yet"; 
      } else {
        if (vm["algorithm"].as<std::string>() == "randomized") {
          int smoothing_p=vm["smoothing_p"].as<int>();
          typename WorkSpaceType::MatrixTable_t projector_table;
          projector_table.Init("",
              std::vector<index_t>(1, references_table->n_attributes()),
              std::vector<index_t>(),
              svd_rank);
          typename WorkSpaceType::MatrixTable_t::Point_t point;
          fl::logger->Message()<<"Generating a gaussian random matrix"
            <<std::endl;
          for(index_t i=0; i<projector_table.n_entries(); ++i) {
            projector_table.get(i, &point); 
            for(index_t j=0; j<point.size(); ++j) {
              point.set(j, fl::math::RandomNormal());
            }
          }
          fl::logger->Message()<<"Gaussian matrix generated"<<std::endl;
          fl::logger->Message()<<"Computing randomized svd"<<std::endl;
          engine.ComputeRandomizedSvd(*references_table,
                               smoothing_p,
                               projector_table,
                               sv_table.get(),
                               left_table.get(),
                               right_trans_table.get());

        } else {
          if (vm["algorithm"].as<std::string>() == "concept") {
            std::vector<double> l2norms(references_table->n_entries());
            if (vm["l2normalize"].as<bool>()==true) {
              fl::logger->Message()<<"L2 normalization of the input "
                "data"<<std::endl;
              typename Table_t::Point_t point;
              for(index_t i=0; i<references_table->n_entries(); ++i) {
                references_table->get(i, &point);
                double norm=fl::la::LengthEuclidean(point);
                l2norms[i]=norm;   
              }
            }
            fl::logger->Message()<<"Computing SVD with Concept "
              "decomposition"<<std::endl;
            index_t n_iterations=vm["n_iterations"].as<index_t>();
            double error_change=0;
            engine.ComputeConceptSvd(*references_table,
                                     l2norms,
                                     n_iterations,
                                     error_change,
                                     sv_table.get(),
                                     left_table.get(),
                                     right_trans_table.get());
          } else {
            fl::logger->Die()<<"This algorithm ("<<
                vm["algorithm"].as<std::string>()
                << ") is not supported";
          }
        }     
      }
    }
  }

  if( vm["rec_error"].as<bool>()==true) {
    double error;
    fl::logger->Message()<<"Computing reconstruction error"<<std::endl;
    fl::logger->Message()<<"referece_table="<<references_table->n_entries()
      <<"x"<<references_table->n_attributes();
    fl::logger->Message()<<"sv_table="<<sv_table->n_entries()
      <<"x"<<sv_table->n_attributes();
    fl::logger->Message()<<"left_table="<<left_table->n_entries()
      <<"x"<<left_table->n_attributes();
    fl::logger->Message()<<"right_trans_table="<<right_trans_table->n_entries()
      <<"x"<<right_trans_table->n_attributes();
    engine.ComputeRecError(*references_table,
                           *sv_table,
                           *left_table,
                           *right_trans_table,
                           &error);
    int truncated_error=static_cast<int>(error*10000);
    fl::logger->Message()<<"The reconstruction error is: "
      <<truncated_error/100.0<<"%"<<std::endl;

  }
  fl::logger->Message() << "Exporting the results of SVD " << std::endl;
  ws->Purge(lsv_file);
  ws->Purge(sv_file);
  ws->Purge(rsv_trans_file);

  ws->Detach(lsv_file);
  ws->Detach(sv_file);
  ws->Detach(rsv_trans_file);
  fl::logger->Message() << "Finished exporting the results of SVD" << std::endl;

  return 0;
}

template<typename WorkSpaceType>
void fl::ml::Svd<boost::mpl::void_>::Run(
      WorkSpaceType *ws,
      const std::vector<std::string> &args) {
  fl::ws::Task<
    WorkSpaceType,
    &Main<
      WorkSpaceType, 
      typename WorkSpaceType::Branch_t
    > 
  > task(ws, args);
  ws->schedule(task); 
}

#endif

