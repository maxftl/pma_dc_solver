#pragma once

#include <lemon/network_simplex.h>
#include <lemon/capacity_scaling.h>
#include <lemon/cycle_canceling.h>
#include <lemon/cost_scaling.h>
#include <lemon/smart_graph.h>

using graph_t = lemon::SmartDigraph;
using node_t = graph_t::Node;
using arc_t = graph_t::Arc;
using arcmap_t = graph_t::ArcMap<int>;
using nodemap_t = graph_t::NodeMap<int>;

//using algorithm_t = lemon::NetworkSimplex<graph_t,int,int>;
using algorithm_t = lemon::CostScaling<graph_t, int, int>;