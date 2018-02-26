/* Copyright (C) 2017
 * Theodoros Chondrogiannis
 * Free University of Bozen-Bolzano
 *
 * This file is part of ParDiSP.
 *
 * ParDiSP is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Affero General Public License
 * as published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 *
 * ParDiSP is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with ParDiSP; see the file COPYING; if not,
 * see <http://www.gnu.org/licenses/>.
 */

#ifndef SUBGRAPHCH_HPP
#define SUBGRAPHCH_HPP

using namespace std;

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

#include <unordered_map>

#include "config.h"
#include "stats/utils.h"
#include "Command.h"
#include "io/createGraph.h"
#include "io/output.h"
#include "datastr/graph/UpdateableGraph.h"
#include "datastr/graph/SearchGraph.h"
#include "datastr/graph/path.h"
#include "processing/DijkstraCH.h"
#include "processing/ConstructCH.h"
#include "manyToMany.h"

typedef processing::ConstructCH<datastr::graph::UpdateableGraph> ProcessingCH;


struct SubgraphCH {
	datastr::graph::UpdateableGraph* updGraph;
	vector<int> mapOrigToTransit;
	vector<int> mapTransitToOrig;
	
	SubgraphCH();
	SubgraphCH(vector<pair<pair<int,int>,int>> &edgesWithWeights, vector<int> &mapOrigToTransit, vector<int> &mapTransitToOrig);

	Matrix<EdgeWeight> manyToMany(vector<NodeID> sources, vector<NodeID> targets);
	vector<NodeID> get_shortest_path(int source, int target);
};

// doesn't look nice, but required by the compiler (gcc 4)
//const EdgeWeight Weight::MAX_VALUE;
//const EliminationWeight::Type EliminationWeight::MAX_VALUE = __DBL_MAX__;
//const EliminationWeight::Type EliminationWeight::MIN_VALUE = -__DBL_MAX__;
//const EdgeWeight Weight::MAX_INTEGER;
//const int datastr::graph::UpdateableGraph::LOOK_FOR_SECOND_EDGE_BACKWARD;
//const int datastr::graph::UpdateableGraph::LOOK_FOR_SECOND_EDGE_FORWARD;

#endif // CONFIG_H
