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

#ifndef FL_LITE_FASTLIB_TREE_ABSTRACT_STATISTIC_H_
#define FL_LITE_FASTLIB_TREE_ABSTRACT_STATISTIC_H_
namespace fl {
namespace tree {
class AbstractStatistic {
  public:
    AbstractStatistic() {

    }

    virtual ~AbstractStatistic() {

    }

    template<typename Archive>
    void serialize(Archive &ar, const unsigned int version) {
    
    }
    /*
          template<typename TreeIterator>
          virtual void Init(TreeIterator &it)=0;

          template<typename TreeIterator>
          virtual void Init(TreeIterator &it,
              const NeighborStatistic& left_stat,
              const NeighborStatistic& right_stat)=0;
    */
};
}
}

#endif

