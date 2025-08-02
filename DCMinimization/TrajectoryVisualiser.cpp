#include "TrajectoryVisualiser.h"
#include <sstream>

using namespace std;


TrajectoryVisualiser::TrajectoryVisualiser()
{
}

void TrajectoryVisualiser::price_trajectory_to_svg(const Auction & auction, const std::vector<Eigen::VectorXd> price_trajectory, const std::string & filename)
{
	int width = 500;
	int height = 500;
	int margin = 0;
	stringstream svg_str;

	int max_price = auction.pos_valuation.maxCoeff();
	int unit = (500 - margin) / max_price;
	int pos_bids_radius = 7;
	int neg_bids_radius = 5;
	int trajectory_length = 10;


	write_header(svg_str,width, height);
	//Plot positive bids
	for (int i = 0; i < auction.num_pos_bids; ++i) {
		int x = auction.pos_valuation(i, 0)*unit;
		int y = height - auction.pos_valuation(i, 1)*unit;
		draw_point(svg_str, x, y, pos_bids_radius, "red", "red");
		draw_line(svg_str, x, y, width, y, "red",3);
		draw_line(svg_str, x, y, x, 0, "red",3);
		draw_line(svg_str, x, y, x-height+y, height, "red",3);
	}
	//Plot negative bids
	for (int i = 0; i < auction.num_neg_bids; ++i) {
		int x = auction.neg_valuation(i, 0)*unit;
		int y = height - auction.neg_valuation(i, 1)*unit;
		draw_point(svg_str, auction.neg_valuation(i, 0)*unit, height - auction.neg_valuation(i, 1)*unit, neg_bids_radius, "blue", "blue");
		draw_line(svg_str, x, y, width, y, "blue");
		draw_line(svg_str, x, y, x, 0, "blue");
		draw_line(svg_str, x, y, x - height + y, height, "blue");
	}
	//Plot trajectory
	int c = 1;
	for (auto it = price_trajectory.begin(); it != price_trajectory.end(); ++it) {
		draw_point(svg_str, unit*(*it)(0), height - unit*(*it)(1), 3, "green", "green");
		draw_text(svg_str, unit*(*it)(0), height - unit * (*it)(1) + 15, std::to_string(c), "green", 20);
		++c;
	}
	write_footer(svg_str);

	ofstream f_str(filename);
	f_str << svg_str.rdbuf();
	f_str.close();
}

void TrajectoryVisualiser::write_header(std::stringstream & s, int width, int height)
{
	s <<
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?> \n" <<
		"<!DOCTYPE svg PUBLIC \" -//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\"> \n" <<
		"<svg xmlns=\"http://www.w3.org/2000/svg\" \n" <<
		"xmlns:xlink = \"http://www.w3.org/1999/xlink\" \n" <<
		"version = \"1.1\" baseProfile = \"full\" " <<
		"width = \"" << width << "mm\" height = \"" << height << "mm\" " <<
		"viewBox = \"-20 -20 " << width << " " << height << "\" > \n" <<
		"<title>Title</title><desc>Description</desc>";
}

void TrajectoryVisualiser::write_footer(std::stringstream & s)
{
	s << "</svg>";
}

void TrajectoryVisualiser::draw_point(std::stringstream & s, int x, int y, int r, std::string fill, std::string stroke)
{
	s << "<circle cx=\"" << x << "\" cy=\"" << y << "\" r=\"" << r << "\" fill=\"" << fill << "\" stroke=\"" << stroke << "\" /> \n";
}

void TrajectoryVisualiser::draw_line(std::stringstream & s, int x1, int y1, int x2, int y2, std::string stroke, int width)
{
	s << "<line x1=\"" << x1 << "\" y1=\"" << y1 << "\" x2=\"" << x2 << "\" y2=\"" << y2 << "\" stroke=\"" << stroke << "\" stroke-width=\"" << width << "\" /> \n";
}

void TrajectoryVisualiser::draw_text(std::stringstream & s, int x, int y, std::string text, std::string stroke, int size)
{
	s << "<text x=\"" << x << "\" y=\"" << y << "\" fill=\"" << stroke << "\" font-size=\"" << size << "px\" >" << text << "</text> \n";
}

void TrajectoryVisualiser::draw_cross(std::stringstream & s, int x, int y, int l)
{
	int x1 = x - l / 2; int x2 = x + l / 2;
	int y1 = y - l / 2; int y2 = y + l / 2;
	s <<
		"<line x1=\"" << x1 << "\" y1=\"" << y1 << "\" x2=\"" << x2 << "\" y2=\"" << y2 << "\" /> \n" <<
		"<line x1=\"" << x1 << "\" y1=\"" << y2 << "\" x2=\"" << x2 << "\" y2=\"" << y1 << "\" /> \n";
}


TrajectoryVisualiser::~TrajectoryVisualiser()
{
}
