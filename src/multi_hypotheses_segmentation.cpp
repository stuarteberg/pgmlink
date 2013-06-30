// stl
#include <vector>
#include <numeric>

// vigra
#include <vigra/multi_array.hxx>

// boost
#include <boost/shared_ptr.hpp>

// armadillo
#include <armadillo>

// pgmlink
#include <pgmlink/multi_hypotheses_segmentation.h>
#include <pgmlink/clustering.h>
#include <pgmlink/traxels.h>
#include "pgmlink/log.h"

namespace pgmlink {
  ////
  //// MultiSegmenter
  ////
  MultiSegmenter::MultiSegmenter(const std::vector<unsigned>& n_clusters,
                                 ClusteringPtr clusterer) :
    n_clusters_(n_clusters),
    clusterer_(clusterer) {}


  vigra::MultiArray<2, label_type> MultiSegmenter::operator()(uint offset) {
    const arma::mat& data_arma_ = clusterer_->get_data_arma();
    unsigned n_samples = data_arma_.n_cols;
    unsigned n_layers = n_clusters_.size();
    vigra::MultiArray<2, label_type> res(vigra::Shape2(n_samples, n_layers), 0u);
    std::vector<unsigned>::const_iterator it = n_clusters_.begin();
    unsigned layer_index = 0;
    unsigned sample_index;
    for (; it != n_clusters_.end(); ++it, ++layer_index) {
      clusterer_->set_k_clusters(*it);
      clusterer_->operator()();
      for (sample_index = 0; sample_index < n_samples; ++sample_index) {
        res(sample_index, layer_index) = assign(data_arma_.col(sample_index)) + offset;
      }
      offset += *it;
    }
    return res;
  }


  label_type MultiSegmenter::assign(const arma::vec& sample) {
    return clusterer_->get_cluster_assignment(sample);
  }


  ////
  //// MultiSegmenterBuilder
  ////
  MultiSegmenterBuilder::MultiSegmenterBuilder(const std::vector<unsigned>& n_clusters,
                                               ClusteringBuilderPtr clustering_builder) :
    n_clusters_(n_clusters),
    clustering_builder_(clustering_builder) {}


  MultiSegmenterPtr MultiSegmenterBuilder::build(const feature_array& data) {
    return MultiSegmenterPtr(new MultiSegmenter(n_clusters_,
                                                clustering_builder_->build(data)
                                                )
                             );
  }


  ////
  //// MultiSegmentContainer
  ////
  MultiSegmentContainer::MultiSegmentContainer(vigra::MultiArrayView<2, label_type> assignments,
                                               const feature_array& coordinates) :
    assignments_(assignments), coordinates_(coordinates) {
    
  }
  int MultiSegmentContainer::to_images(vigra::MultiArrayView<4, label_type> dest) {
    if (coordinates_.size()/3 != assignments_.shape()[0]) {
      // number of samples in coordinates differ from number of samples
      // in assignments
      return 1;
    }
    
    if (assignments_.shape()[1] != dest.shape()[3]) {
      // number of segmentation layers does not fit destination image
      return 2;
    }
    
    unsigned n_layers = dest.shape()[3];
    unsigned n_samples = assignments_.shape()[0];
    unsigned sample_coord_index;
    unsigned coord1, coord2, coord3;
    for (unsigned layer = 0; layer < n_layers; ++layer) {
      for (unsigned sample = 0; sample < n_samples; ++sample) {
        sample_coord_index = 3*sample;
        coord1 = coordinates_[sample_coord_index++];
        coord2 = coordinates_[sample_coord_index++];
        coord3 = coordinates_[sample_coord_index];
        dest(coord1, coord2, coord3, layer) = assignments_(sample, layer);
      }
    }

    // regular return
    return 0;
  }


  ////
  //// ConnectedComponentsToMultiSegments
  ////
  ConnectedComponentsToMultiSegments::
  ConnectedComponentsToMultiSegments(const std::vector<feature_array >& components_coordinates,
                                     const std::vector<unsigned>& n_clusters,
                                     ClusteringBuilderPtr clustering_builder,
                                     unsigned starting_index) :
    components_coordinates_(components_coordinates),
    n_clusters_(n_clusters),
    clustering_builder_(clustering_builder),
    starting_index_(starting_index) {

  }


  AssignmentListPtr ConnectedComponentsToMultiSegments::get_assignments() {
    if (assignments_) {
      return assignments_;
    } else {
      assignments_ = AssignmentListPtr(new std::vector<vigra::MultiArray<2, unsigned> >);
      MultiSegmenterPtr segmenter;
      unsigned offset = starting_index_;
      unsigned offset_step = std::accumulate(n_clusters_.begin(), n_clusters_.end(), 0);
      std::vector<feature_array >::const_iterator cc_it = components_coordinates_.begin();
      for (; cc_it != components_coordinates_.end(); ++cc_it) {
        MultiSegmenterBuilder segmenter_builder(n_clusters_, clustering_builder_);
        segmenter = segmenter_builder.build(*cc_it);
        assignments_->push_back(segmenter->operator()(offset));
        offset += offset_step;
      }
      return assignments_;
    }
  }


  void ConnectedComponentsToMultiSegments::to_images(vigra::MultiArray<4, unsigned>& dest) {
    get_assignments();
    assert(components_coordinates_.size() == assignments_->size());
    std::vector<feature_array >::const_iterator cc_it = components_coordinates_.begin();
    std::vector<vigra::MultiArray<2, unsigned> >::iterator as_it = assignments_->begin();
    for (; cc_it != components_coordinates_.end(); ++cc_it, ++as_it) {
      MultiSegmentContainer segment_container(*as_it, *cc_it);
      segment_container.to_images(dest);
    }
  }
}
