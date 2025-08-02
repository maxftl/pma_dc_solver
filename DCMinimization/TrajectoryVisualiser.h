#pragma once
#include <Eigen/Dense>
#include <vector>
#include <string>
#include "Auction.h"
#include <sstream>

class TrajectoryVisualiser
{
public:
	TrajectoryVisualiser();
	void price_trajectory_to_svg(const Auction &auction, const std::vector<Eigen::VectorXd> price_trajectory, const std::string &filename);
	void write_header(std::stringstream &s, int width, int height);
	void write_footer(std::stringstream &s);
	void draw_point(std::stringstream &s, int x, int y, int r, std::string fill = "black", std::string stroke = "black");
	void draw_line(std::stringstream &s, int x1, int y1, int x2, int y2, std::string stroke = "black", int width = 1);
	void draw_text(std::stringstream &s, int x, int y, std::string text, std::string stroke = "black", int size = 10);
	void draw_cross(std::stringstream &s, int x, int y, int l);
	~TrajectoryVisualiser();
};

