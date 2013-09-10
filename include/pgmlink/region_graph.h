#ifndef REGION_GRAPH_H
#define REGION_GRAPH_H

// stl
#include <set>
#include <map>
#include <algorithm>
#include <functional>

// boost
#include <boost/shared_ptr.hpp>

// lemon
#include <lemon/list_graph.h> // lemon::ListGraph
#include <lemon/maps.h> // lemon::IterableValueMap


// pgmlink
#include <pgmlink/graph.h>
#include <pgmlink/hypotheses.h> // node_traxel


namespace pgmlink {

  class RegionGraph;

  template <typename Graph, typename Key, typename Value>
  class IterableEditableValueMap;

  typedef boost::shared_ptr<RegionGraph> RegionGraphPtr;

  typedef unsigned label_type;

  

  ////
  //// IterableEditableValueMap
  ////
  template <typename Graph, typename Key, typename Value>
  class IterableEditableValueMap : public lemon::IterableValueMap<Graph, Key, Value> {
  private:
  public:
    Value& get_value(const Key& key);
    explicit IterableEditableValueMap(const Graph& graph,
                                      const Value& value = Value());
  };

  template <typename Graph, typename Key, typename Value>
  IterableEditableValueMap<Graph, Key, Value>::IterableEditableValueMap(const Graph& graph,
                                                                        const Value& value) :
    lemon::IterableValueMap<Graph,Key, Value>(graph, value) {
    
  }

  template <typename Graph, typename Key, typename Value>
  Value& IterableEditableValueMap<Graph, Key, Value>::get_value(const Key& key) {
    return lemon::IterableValueMap<Graph, Key, Value>::Parent::operator[](key).value;
  }


  ////
  //// node_neighbors
  ////  
  struct node_neighbors {};
  template <typename Graph>
  struct property_map<node_neighbors, Graph> {
    typedef IterableEditableValueMap<Graph, typename Graph::Node, std::set<typename Graph::Node> > type;
    static const std::string name;
  };
  
  template <typename Graph>
  const std::string property_map<node_neighbors, Graph>::name = "node_neighbors";


  ////
  //// arc_dissimilarity
  ////
  struct arc_dissimilarity {};
  template <typename Graph>
  struct property_map<arc_dissimilarity, Graph> {
    typedef lemon::IterableValueMap<Graph, typename Graph::Arc, double> type;
    static const std::string name;
  };

  template <typename Graph>
  const std::string property_map<arc_dissimilarity, Graph>::name = "arc_dissimilarity";


  ////
  //// node_label
  ////
  struct node_label {};
  template <typename Graph>
  struct property_map<node_label, Graph> {
    typedef lemon::CrossRefMap<Graph, typename Graph::Node, label_type> type;
    static const std::string name;
  };

  template <typename Graph>
  const std::string property_map<node_label, Graph>::name = "node_label";


  ////
  //// node_contains
  ////
  struct node_contains {};
  template <typename Graph>
  struct property_map<node_contains, Graph> {
    typedef IterableEditableValueMap<Graph, typename Graph::Node, std::set<typename Graph::Node> > type;
    static const std::string name;
  };

  template <typename Graph>
  const std::string property_map<node_contains, Graph>::name = "node_contains";
  

  ////
  //// node_level
  ////
  struct node_level {};
  template <typename Graph>
  struct property_map<node_level, Graph> {
    typedef lemon::IterableValueMap<Graph, typename Graph::Node, unsigned> type;
    static const std::string name;
  };

  template <typename Graph>
  const std::string property_map<node_level, Graph>::name = "node_level";



  ////
  //// node_conflicts
  ////
  struct node_conflicts {};
  template <typename Graph>
  struct property_map<node_conflicts, Graph> {
    typedef IterableEditableValueMap<Graph, typename Graph::Node, std::set<typename Graph::Node> > type;
    static const std::string name;
  };

  template <typename Graph>
  const std::string property_map<node_conflicts, Graph>::name = "node_conflicts";


  ////
  //// node_connceted_component
  ////
  struct node_connected_component {};
  template <typename Graph>
  struct property_map<node_connected_component, Graph> {
    typedef lemon::IterableValueMap<Graph, typename Graph::Node, label_type> type;
    static const std::string name;
  };

  template <typename Graph>
  const std::string property_map<node_connected_component, Graph>::name = "node_connected_component";


  ////
  //// node_merged_n_times
  ////
  struct node_merged_n_times {};
  template <typename Graph>
  struct property_map<node_merged_n_times, Graph> {
    typedef IterableEditableValueMap<Graph, typename Graph::Node, unsigned> type;
    static const std::string name;
  };

  template <typename Graph>
  const std::string property_map<node_merged_n_times, Graph>::name = "node_merged_n_times";


  ////
  //// connected_component_merged_n_times
  ////
  struct connected_component_merged_n_times {};
  template <typename Graph>
  struct property_map<connected_component_merged_n_times, Graph> {
    typedef IterableEditableValueMap<Graph, label_type, unsigned> type;
    static const std::string name;
  };

  template <typename Graph>
  const std::string property_map<connected_component_merged_n_times, Graph>::name =
    "connected_component_merged_n_times";


  ////
  //// RegionGraph
  ////
  class RegionGraph : public PropertyGraph<lemon::ListGraph> {
  public:
    typedef base_graph::Node Region;
    typedef base_graph::Arc Dissimilarity;
    typedef property_map<node_neighbors, base_graph>::type NeighborMap;
    typedef property_map<arc_dissimilarity, base_graph>::type DissimilarityMap;
    typedef property_map<node_label, base_graph>::type LabelMap;
    typedef property_map<node_contains, base_graph>::type ContainingMap;
    typedef property_map<node_conflicts, base_graph>::type ConflictMap;
    typedef property_map<node_level, base_graph>::type LevelMap;
    typedef property_map<node_connected_component, base_graph>::type ConnectedComponentMap;
    typedef property_map<node_merged_n_times, base_graph>::type NumberMergesMap;
    typedef property_map<node_traxel, base_graph>::type TraxelMap;
    typedef std::vector<base_graph::Node> NodeVector;
    typedef boost::shared_ptr<NodeVector> NodeVectorPtr;
  private:
    label_type maximum_label_;
    void union_neighbors(const Region& r1,
                         const Region& r2,
                         const Region& new_region);
    void union_conflicts(const Region& r1,
                         const Region& r2,
                         const Region& new_region);
  public:
    RegionGraph();
    Region merge_regions(Region r1, Region r2);
    Region merge_regions(label_type label1, label_type label2);
    Region add_region(label_type label);
    int get_maximum_label();
    NodeVectorPtr get_nodes_in_component(unsigned component_label);
    NodeVectorPtr get_connected_components();
    template <typename PropertyTag, typename Iterator>
    boost::shared_ptr<std::vector<typename property_map<PropertyTag, RegionGraph::base_graph>::type::Value> >
    convert_nodes_to_property(Iterator start, Iterator end);
  };


template<typename PropertyTag, typename Iterator>
boost::shared_ptr<std::vector<typename property_map<PropertyTag, RegionGraph::base_graph>::type::Value> >
RegionGraph::convert_nodes_to_property(Iterator start, Iterator end) {
  boost::shared_ptr<std::vector<typename property_map<PropertyTag, RegionGraph::base_graph>::type::Value> >
      properties(new std::vector<typename property_map<PropertyTag, RegionGraph::base_graph>::type::Value>);
  typename property_map<PropertyTag, RegionGraph::base_graph>::type& map = get(PropertyTag());
  for (; start != end; ++start) {
    properties->push_back(map[*start]);
  }
  return properties;
}

  
  template <typename InputIterator, typename OutputIterator, typename UnaryPredicate>
  OutputIterator conditional_copy (InputIterator first, InputIterator last,
                                   OutputIterator result, UnaryPredicate pred) {
    while (first!=last) {
      if (pred(*first)) {
        *result = *first;
        ++result;
      }
      ++first;
    }
    return result;
  }

  template <typename InputIterator, typename ComparisonValue>
  class IsContainedFunctor : public std::unary_function<ComparisonValue, bool> {
  private:
    InputIterator begin_;
    InputIterator end_;
    IsContainedFunctor();
  public:
    IsContainedFunctor(InputIterator begin,
                       InputIterator end) :
      begin_(begin),
      end_(end) {}
    bool operator()(const ComparisonValue& compare) const {
      return !(std::find(begin_, end_, compare) == end_);
    }
  };

  typedef IsContainedFunctor<std::set<RegionGraph::Region>::iterator,
                             RegionGraph::Region> IsInRegionSet;

}





#endif /* REGION_GRAPH_H */