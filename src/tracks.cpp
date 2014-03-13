#include "pgmlink/tracks.h"

namespace pgmlink {

////
//// Class Track
////
Track::Track() {
  parent_id_ = 0;
  child_ids_.resize(2,0);
}

Track::Track(
  const size_t id,
  const size_t time_start,
  const Traxelvector& traxels,
  const size_t parent_id,
  const size_t left_child_id,
  const size_t right_child_id
) : id_(id), time_start_(time_start), traxels_(traxels), parent_id_(parent_id) {
  Track::set_child_ids(left_child_id, right_child_id);
}

void Track::set_id(const size_t id) {
  id_ = id;
}

size_t Track::get_id() const {
  return Track::id_;
}

void Track::set_time_start(const size_t time_start) {
  time_start_ = time_start;
}

size_t Track::get_time_start() const {
  return Track::time_start_;
}

void Track::set_parent_id(const size_t parent_id) {
  parent_id_ = parent_id;
}

size_t Track::get_parent_id() const {
  return Track::parent_id_;
}

void Track::set_child_ids(const size_t left, const size_t right) {
  child_ids_.resize(2);
  child_ids_[0] = left;
  child_ids_[1] = right;
}

const std::vector<size_t>& Track::get_child_ids() const {
  return Track::child_ids_;
}

size_t Track::get_length() const {
  return Track::traxels_.size();
}

////
//// Class TrackFeatureExtractor
////
TrackFeatureExtractor::TrackFeatureExtractor(
  boost::shared_ptr<FeatureCalculator> calculator,
  const std::string& feature_name,
  const FeatureOrder order 
) :
  calculator_(calculator),
  extractor_(new FeatureExtractor(calculator, feature_name)),
  feature_name_(feature_name),
  order_(order) {
}

TrackFeatureExtractor::TrackFeatureExtractor(
  boost::shared_ptr<FeatureCalculator> calculator,
  boost::shared_ptr<FeatureAggregator> aggregator,
  const std::string& feature_name,
  const FeatureOrder order
) :
  calculator_(calculator),
  extractor_(new FeatureExtractor(calculator, feature_name)),
  aggregator_(aggregator),
  feature_name_(feature_name),
  order_(order) {
}

feature_arrays TrackFeatureExtractor::extract(const Track& track) const {
  assert(track.get_length - order_ > 0);

  feature_arrays ret;
  Traxelvector::const_iterator traxel_it;

  for (
    traxel_it = track.traxels_.begin();
    traxel_it+order_ != track.traxels_.end();
    traxel_it++
  ) {
    switch(order_) {
      case SINGLE:
        ret.push_back(extractor_->extract(*traxel_it));
        break;
      case PAIRWISE:
        ret.push_back(extractor_->extract(*traxel_it, *(traxel_it+1)));
        break;
      case TRIPLET:
        ret.push_back(extractor_->extract(*traxel_it, *(traxel_it+1), *(traxel_it+2)));
    }
  }
  return ret;
}

feature_array TrackFeatureExtractor::extract_vector(const Track& track) const {
  if (!aggregator_) {
    throw std::runtime_error (
      "No aggregator set in TrackFeatureExtractor"
    );
  }
  feature_arrays features = TrackFeatureExtractor::extract(track);
  return aggregator_->vector_valued(features);
}

feature_type TrackFeatureExtractor::extract_scalar(const Track& track) const {
  if (!aggregator_) {
    throw std::runtime_error (
      "No aggregator set in TrackFeatureExtractor"
    );
  }
  feature_arrays features = TrackFeatureExtractor::extract(track);
  return aggregator_->scalar_valued(features);
} 

FeatureOrder TrackFeatureExtractor::get_feature_order() const {
  return TrackFeatureExtractor::order_;
}


} // namespace pgmlink