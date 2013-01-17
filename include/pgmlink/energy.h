/**
   @file
   @ingroup pgm
   @brief energy functions
*/

#ifndef ENERGY_H
#define ENERGY_H

#include <stdexcept>
#include "pgmlink/traxels.h"
#include <cmath>

namespace pgmlink {

class GeometryDivision2 {
  /**
   * Division Energy based on geometric properties of a ancestor-child-child configuration.
   *
   * The energy is the sum of the squared distances from the ancestor object to the two children shifted by
   * a mean distance [distance dependence].
   * Furthermore, there may be an additional hard constraint on the division angle. If the angle is to small,
   * the energy will be set to a very high value. [angle constraint]
   */
 public:
  /**
   * @param mean_div_dist expected moving distance during divisions
   * @param min_angle minimal angle to accept configuration as a division
   * @param distance_dependence if turned off, energy will not depend on distance between ancestor and children
   * @param angle_constraint if turned on, a minimal division angle is demanded
   */
 GeometryDivision2(double mean_div_dist, double min_angle, bool distance_dependence=true, bool angle_constraint=true) 
   : mean_div_dist_(mean_div_dist), min_angle_(min_angle), 
    distance_dependence_(distance_dependence), angle_constraint_(angle_constraint) {};
    double operator()(const Traxel& ancestor,
	const Traxel& child1,
	const Traxel& child2) const;
    private:
    double mean_div_dist_, min_angle_;
    bool distance_dependence_, angle_constraint_; 
};

class KasterDivision2 {
 public:
 KasterDivision2(double weight, double div_cost) : w_(weight), div_cost_(div_cost) {};
    double operator()(const Traxel& ancestor,
	const Traxel& child1,
	const Traxel& child2) const;
 private:
   double w_;
   double div_cost_;
 };

class NegLnCellness {
 public:
 NegLnCellness(double weight) : w_(weight) {}
  double operator()( const Traxel& ) const;
 private:
  double w_;
};

class NegLnOneMinusCellness {
 public:
 NegLnOneMinusCellness(double weight) : w_(weight) {}
  double operator()( const Traxel& ) const;
 private:
  double w_;
};

class BorderAwareConstant {
  /**
   * Zero near temporal border, else 1.
   */
 public:
 BorderAwareConstant( double weight,
		       int at,
		       bool early,
		       int margin_t=1) 
   : w_(weight), at_(at), early_(early), margin_t_(margin_t) {}

  double operator()( const Traxel& tr ) const {
    int t = tr.Timestep;
    if(early_) {
      if( at_ <= t && t < at_ + margin_t_) { 
	return 0;
      } else return w_;
    } else {
      if (at_ - margin_t_ < t && t <= at_) {
	return 0.;
      } else return w_;
    }
  }

 private:
  double w_;
  int at_;
  bool early_;
  int margin_t_;
};

/**
   @brief Primitive fixed cost energy functor
 */
 class ConstantEnergy {
 public:
   ConstantEnergy( double energy = 0. );

   double operator()( const Traxel& t1,
		      const Traxel& t2,
		      const Traxel& t3 ) const { return theEnergy; }

   double operator()( const Traxel& t1,
		      const Traxel& t2) const { return theEnergy; }

   double operator()( const Traxel& t ) const { return theEnergy; }

   double operator()() const { return theEnergy; };

   double theEnergy;
 };


    
    class SquaredDistance : public BinaryEnergy {
	public:
        virtual double operator()(const Traxel& from,
                          const Traxel& to,
                          const Traxels& prev,
                          const Traxels& curr) const;
    };


    class KasterDivision : public TertiaryEnergy {
	public:
	KasterDivision(double division_cost) : div_cost_(division_cost) {};

        virtual double operator()(const Traxel& ancestor,
                          const Traxel& child1,
                          const Traxel& child2,
                          const Traxels& prev,
                          const Traxels& curr) const;
	
	private:
	double div_cost_;
    };

    class GeometryDivision : public TertiaryEnergy {
	public:
	            virtual double operator()(const Traxel& ancestor,
                          const Traxel& child1,
                          const Traxel& child2,
                          const Traxels& prev,
                          const Traxels& curr) const;
    };

    ////
    //// Cellness based mlinder-type energies
    ////
    class CellnessDivision : public TertiaryEnergy {
	public:
        CellnessDivision( double diffCellness = 1461, double absCellness = 190 );

        virtual double operator()(const Traxel& ancestor,
                                  const Traxel& child1,
                                  const Traxel& child2,
                                  const Traxels& prev,
                                  const Traxels& curr) const;
    private:
        // weight for the difference of daughter cells' cellness
        double param_diff_c;
        // weight for the absolute cellness of the parent cell
        double param_abs_c;
    };



    class CellnessMove : public BinaryEnergy {
	public:
        CellnessMove( double diffCellness = 1140 );

        virtual double operator()(const Traxel& from,
                                  const Traxel& to,
                                  const Traxels& prev,
                                  const Traxels& curr) const;
    private:
        // weight for the difference of the cells' cellness
        double param_diff_c;
    };



    class CellnessDisappearance : public UnaryEnergy {
	public:
        CellnessDisappearance( double disappWeight = 1000 );

        virtual double operator()(const Traxel& from,
                                  const Traxels& prev,
                                  const Traxels& curr) const;
    private:
        // weight for the absolute cellness
        double param_abs_c;
    };



    class CellnessAppearance : public UnaryEnergy {
	public:
        CellnessAppearance( double appWeight = 1000 );

        virtual double operator()(const Traxel& to,
                                  const Traxels& prev,
                                  const Traxels& curr) const;
    private:
        // weight for the absolute cellness
        double param_abs_c;
    };

} /* Namespace pgmlink */

#endif /* ENERGY_H */
