#ifndef BOOST_GEOMETRY_PROJECTIONS_AEQD_HPP
#define BOOST_GEOMETRY_PROJECTIONS_AEQD_HPP

// Boost.Geometry - extensions-gis-projections (based on PROJ4)
// This file is automatically generated. DO NOT EDIT.

// Copyright (c) 2008-2015 Barend Gehrels, Amsterdam, the Netherlands.

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// This file is converted from PROJ4, http://trac.osgeo.org/proj
// PROJ4 is originally written by Gerald Evenden (then of the USGS)
// PROJ4 is maintained by Frank Warmerdam
// PROJ4 is converted to Boost.Geometry by Barend Gehrels

// Last updated version of proj: 4.9.1

// Original copyright notice:
 
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.


#include <boost/math/special_functions/hypot.hpp>

#include <boost/geometry/extensions/gis/projections/impl/base_static.hpp>
#include <boost/geometry/extensions/gis/projections/impl/base_dynamic.hpp>
#include <boost/geometry/extensions/gis/projections/impl/projects.hpp>
#include <boost/geometry/extensions/gis/projections/impl/factory_entry.hpp>
#include <boost/geometry/extensions/gis/projections/impl/aasincos.hpp>
#include <boost/geometry/extensions/gis/projections/impl/pj_mlfn.hpp>
#include <boost/geometry/extensions/gis/projections/impl/proj_mdist.hpp>

namespace boost { namespace geometry { namespace projections
{
    #ifndef DOXYGEN_NO_DETAIL
    namespace detail { namespace aeqd{
            static const double EPS10 = 1.e-10;
            static const double TOL = 1.e-14;
            static const int N_POLE = 0;
            static const int S_POLE = 1;
            static const int EQUIT = 2;
            static const int OBLIQ = 3;

            struct par_aeqd
            {
                double    sinph0;
                double    cosph0;
                double    en[EN_SIZE];
                double    M1;
                double    N1;
                double    Mp;
                double    He;
                double    G;
                int        mode;
            };

            // template class, using CRTP to implement forward/inverse
            template <typename Geographic, typename Cartesian, typename Parameters>
            struct base_aeqd_ellipsoid : public base_t_fi<base_aeqd_ellipsoid<Geographic, Cartesian, Parameters>,
                     Geographic, Cartesian, Parameters>
            {

                 typedef double geographic_type;
                 typedef double cartesian_type;

                par_aeqd m_proj_parm;

                inline base_aeqd_ellipsoid(const Parameters& par)
                    : base_t_fi<base_aeqd_ellipsoid<Geographic, Cartesian, Parameters>,
                     Geographic, Cartesian, Parameters>(*this, par) {}

                inline void fwd(geographic_type& lp_lon, geographic_type& lp_lat, cartesian_type& xy_x, cartesian_type& xy_y) const
                {
                    double  coslam, cosphi, sinphi, rho, s, H, H2, c, Az, t, ct, st, cA, sA;

                    coslam = cos(lp_lon);
                    cosphi = cos(lp_lat);
                    sinphi = sin(lp_lat);
                    switch (this->m_proj_parm.mode) {
                    case N_POLE:
                        coslam = - coslam;
                    case S_POLE:
                        xy_x = (rho = fabs(this->m_proj_parm.Mp - pj_mlfn(lp_lat, sinphi, cosphi, this->m_proj_parm.en))) *
                            sin(lp_lon);
                        xy_y = rho * coslam;
                        break;
                    case EQUIT:
                    case OBLIQ:
                        if (fabs(lp_lon) < EPS10 && fabs(lp_lat - this->m_par.phi0) < EPS10) {
                            xy_x = xy_y = 0.;
                            break;
                        }
                        t = atan2(this->m_par.one_es * sinphi + this->m_par.es * this->m_proj_parm.N1 * this->m_proj_parm.sinph0 *
                            sqrt(1. - this->m_par.es * sinphi * sinphi), cosphi);
                        ct = cos(t); st = sin(t);
                        Az = atan2(sin(lp_lon) * ct, this->m_proj_parm.cosph0 * st - this->m_proj_parm.sinph0 * coslam * ct);
                        cA = cos(Az); sA = sin(Az);
                        s = aasin(fabs(sA) < TOL ?
                            (this->m_proj_parm.cosph0 * st - this->m_proj_parm.sinph0 * coslam * ct) / cA :
                            sin(lp_lon) * ct / sA );
                        H = this->m_proj_parm.He * cA;
                        H2 = H * H;
                        c = this->m_proj_parm.N1 * s * (1. + s * s * (- H2 * (1. - H2)/6. +
                            s * ( this->m_proj_parm.G * H * (1. - 2. * H2 * H2) / 8. +
                            s * ((H2 * (4. - 7. * H2) - 3. * this->m_proj_parm.G * this->m_proj_parm.G * (1. - 7. * H2)) /
                            120. - s * this->m_proj_parm.G * H / 48.))));
                        xy_x = c * sA;
                        xy_y = c * cA;
                        break;
                    }
                }

                inline void inv(cartesian_type& xy_x, cartesian_type& xy_y, geographic_type& lp_lon, geographic_type& lp_lat) const
                {
                    double c, Az, cosAz, A, B, D, E, F, psi, t;

                    if ((c = boost::math::hypot(xy_x, xy_y)) < EPS10) {
                        lp_lat = this->m_par.phi0;
                        lp_lon = 0.;
                            return;
                    }
                    if (this->m_proj_parm.mode == OBLIQ || this->m_proj_parm.mode == EQUIT) {
                        cosAz = cos(Az = atan2(xy_x, xy_y));
                        t = this->m_proj_parm.cosph0 * cosAz;
                        B = this->m_par.es * t / this->m_par.one_es;
                        A = - B * t;
                        B *= 3. * (1. - A) * this->m_proj_parm.sinph0;
                        D = c / this->m_proj_parm.N1;
                        E = D * (1. - D * D * (A * (1. + A) / 6. + B * (1. + 3.*A) * D / 24.));
                        F = 1. - E * E * (A / 2. + B * E / 6.);
                        psi = aasin(this->m_proj_parm.sinph0 * cos(E) + t * sin(E));
                        lp_lon = aasin(sin(Az) * sin(E) / cos(psi));
                        if ((t = fabs(psi)) < EPS10)
                            lp_lat = 0.;
                        else if (fabs(t - HALFPI) < 0.)
                            lp_lat = HALFPI;
                        else
                            lp_lat = atan((1. - this->m_par.es * F * this->m_proj_parm.sinph0 / sin(psi)) * tan(psi) /
                                this->m_par.one_es);
                    } else { /* Polar */
                        lp_lat = pj_inv_mlfn(this->m_proj_parm.mode == N_POLE ? this->m_proj_parm.Mp - c : this->m_proj_parm.Mp + c,
                            this->m_par.es, this->m_proj_parm.en);
                        lp_lon = atan2(xy_x, this->m_proj_parm.mode == N_POLE ? -xy_y : xy_y);
                    }
                }
            };

            // template class, using CRTP to implement forward/inverse
            template <typename Geographic, typename Cartesian, typename Parameters>
            struct base_aeqd_guam : public base_t_fi<base_aeqd_guam<Geographic, Cartesian, Parameters>,
                     Geographic, Cartesian, Parameters>
            {

                 typedef double geographic_type;
                 typedef double cartesian_type;

                par_aeqd m_proj_parm;

                inline base_aeqd_guam(const Parameters& par)
                    : base_t_fi<base_aeqd_guam<Geographic, Cartesian, Parameters>,
                     Geographic, Cartesian, Parameters>(*this, par) {}

                inline void fwd(geographic_type& lp_lon, geographic_type& lp_lat, cartesian_type& xy_x, cartesian_type& xy_y) const
                {
                    double  cosphi, sinphi, t;

                    cosphi = cos(lp_lat);
                    sinphi = sin(lp_lat);
                    t = 1. / sqrt(1. - this->m_par.es * sinphi * sinphi);
                    xy_x = lp_lon * cosphi * t;
                    xy_y = pj_mlfn(lp_lat, sinphi, cosphi, this->m_proj_parm.en) - this->m_proj_parm.M1 +
                        .5 * lp_lon * lp_lon * cosphi * sinphi * t;
                }

                inline void inv(cartesian_type& xy_x, cartesian_type& xy_y, geographic_type& lp_lon, geographic_type& lp_lat) const
                {
                    double x2, t;
                    int i;

                    x2 = 0.5 * xy_x * xy_x;
                    lp_lat = this->m_par.phi0;
                    for (i = 0; i < 3; ++i) {
                        t = this->m_par.e * sin(lp_lat);
                        lp_lat = pj_inv_mlfn(this->m_proj_parm.M1 + xy_y -
                            x2 * tan(lp_lat) * (t = sqrt(1. - t * t)), this->m_par.es, this->m_proj_parm.en);
                    }
                    lp_lon = xy_x * t / cos(lp_lat);
                }
            };

            // template class, using CRTP to implement forward/inverse
            template <typename Geographic, typename Cartesian, typename Parameters>
            struct base_aeqd_spheroid : public base_t_fi<base_aeqd_spheroid<Geographic, Cartesian, Parameters>,
                     Geographic, Cartesian, Parameters>
            {

                 typedef double geographic_type;
                 typedef double cartesian_type;

                par_aeqd m_proj_parm;

                inline base_aeqd_spheroid(const Parameters& par)
                    : base_t_fi<base_aeqd_spheroid<Geographic, Cartesian, Parameters>,
                     Geographic, Cartesian, Parameters>(*this, par) {}

                inline void fwd(geographic_type& lp_lon, geographic_type& lp_lat, cartesian_type& xy_x, cartesian_type& xy_y) const
                {
                    double  coslam, cosphi, sinphi;

                    sinphi = sin(lp_lat);
                    cosphi = cos(lp_lat);
                    coslam = cos(lp_lon);
                    switch (this->m_proj_parm.mode) {
                    case EQUIT:
                        xy_y = cosphi * coslam;
                        goto oblcon;
                    case OBLIQ:
                        xy_y = this->m_proj_parm.sinph0 * sinphi + this->m_proj_parm.cosph0 * cosphi * coslam;
                oblcon:
                        if (fabs(fabs(xy_y) - 1.) < TOL)
                            if (xy_y < 0.)
                                throw proj_exception();
                            else
                                xy_x = xy_y = 0.;
                        else {
                            xy_y = acos(xy_y);
                            xy_y /= sin(xy_y);
                            xy_x = xy_y * cosphi * sin(lp_lon);
                            xy_y *= (this->m_proj_parm.mode == EQUIT) ? sinphi :
                                   this->m_proj_parm.cosph0 * sinphi - this->m_proj_parm.sinph0 * cosphi * coslam;
                        }
                        break;
                    case N_POLE:
                        lp_lat = -lp_lat;
                        coslam = -coslam;
                    case S_POLE:
                        if (fabs(lp_lat - HALFPI) < EPS10) throw proj_exception();;
                        xy_x = (xy_y = (HALFPI + lp_lat)) * sin(lp_lon);
                        xy_y *= coslam;
                        break;
                    }
                }

                inline void inv(cartesian_type& xy_x, cartesian_type& xy_y, geographic_type& lp_lon, geographic_type& lp_lat) const
                {
                    double cosc, c_rh, sinc;

                    if ((c_rh = boost::math::hypot(xy_x, xy_y)) > PI) {
                        if (c_rh - EPS10 > PI) throw proj_exception();;
                        c_rh = PI;
                    } else if (c_rh < EPS10) {
                        lp_lat = this->m_par.phi0;
                        lp_lon = 0.;
                            return;
                    }
                    if (this->m_proj_parm.mode == OBLIQ || this->m_proj_parm.mode == EQUIT) {
                        sinc = sin(c_rh);
                        cosc = cos(c_rh);
                        if (this->m_proj_parm.mode == EQUIT) {
                                        lp_lat = aasin(xy_y * sinc / c_rh);
                            xy_x *= sinc;
                            xy_y = cosc * c_rh;
                        } else {
                            lp_lat = aasin(cosc * this->m_proj_parm.sinph0 + xy_y * sinc * this->m_proj_parm.cosph0 /
                                c_rh);
                            xy_y = (cosc - this->m_proj_parm.sinph0 * sin(lp_lat)) * c_rh;
                            xy_x *= sinc * this->m_proj_parm.cosph0;
                        }
                        lp_lon = atan2(xy_x, xy_y);
                    } else if (this->m_proj_parm.mode == N_POLE) {
                        lp_lat = HALFPI - c_rh;
                        lp_lon = atan2(xy_x, -xy_y);
                    } else {
                        lp_lat = c_rh - HALFPI;
                        lp_lon = atan2(xy_x, xy_y);
                    }
                }
            };

            // Azimuthal Equidistant
            template <typename Parameters>
            void setup_aeqd(Parameters& par, par_aeqd& proj_parm)
            {
                par.phi0 = pj_param(par.params, "rlat_0").f;
                if (fabs(fabs(par.phi0) - HALFPI) < EPS10) {
                    proj_parm.mode = par.phi0 < 0. ? S_POLE : N_POLE;
                    proj_parm.sinph0 = par.phi0 < 0. ? -1. : 1.;
                    proj_parm.cosph0 = 0.;
                } else if (fabs(par.phi0) < EPS10) {
                    proj_parm.mode = EQUIT;
                    proj_parm.sinph0 = 0.;
                    proj_parm.cosph0 = 1.;
                } else {
                    proj_parm.mode = OBLIQ;
                    proj_parm.sinph0 = sin(par.phi0);
                    proj_parm.cosph0 = cos(par.phi0);
                }
                if (! par.es) {
                // par.inv = s_inverse;
                // par.fwd = s_forward;
                } else {
                    pj_enfn(par.es, proj_parm.en);
                    if (pj_param(par.params, "bguam").i) {
                        proj_parm.M1 = pj_mlfn(par.phi0, proj_parm.sinph0, proj_parm.cosph0, proj_parm.en);
                // par.inv = e_guam_inv;
                // par.fwd = e_guam_fwd;
                    } else {
                        switch (proj_parm.mode) {
                        case N_POLE:
                            proj_parm.Mp = pj_mlfn(HALFPI, 1., 0., proj_parm.en);
                            break;
                        case S_POLE:
                            proj_parm.Mp = pj_mlfn(-HALFPI, -1., 0., proj_parm.en);
                            break;
                        case EQUIT:
                        case OBLIQ:
                // par.inv = e_inverse;
                // par.fwd = e_forward;
                            proj_parm.N1 = 1. / sqrt(1. - par.es * proj_parm.sinph0 * proj_parm.sinph0);
                            proj_parm.G = proj_parm.sinph0 * (proj_parm.He = par.e / sqrt(par.one_es));
                            proj_parm.He *= proj_parm.cosph0;
                            break;
                        }
                // par.inv = e_inverse;
                // par.fwd = e_forward;
                    }
                }
            }

        }} // namespace detail::aeqd
    #endif // doxygen

    /*!
        \brief Azimuthal Equidistant projection
        \ingroup projections
        \tparam Geographic latlong point type
        \tparam Cartesian xy point type
        \tparam Parameters parameter type
        \par Projection characteristics
         - Azimuthal
         - Spheroid
         - Ellipsoid
         - lat_0 guam
        \par Example
        \image html ex_aeqd.gif
    */
    template <typename Geographic, typename Cartesian, typename Parameters = parameters>
    struct aeqd_ellipsoid : public detail::aeqd::base_aeqd_ellipsoid<Geographic, Cartesian, Parameters>
    {
        inline aeqd_ellipsoid(const Parameters& par) : detail::aeqd::base_aeqd_ellipsoid<Geographic, Cartesian, Parameters>(par)
        {
            detail::aeqd::setup_aeqd(this->m_par, this->m_proj_parm);
        }
    };

    /*!
        \brief Azimuthal Equidistant projection
        \ingroup projections
        \tparam Geographic latlong point type
        \tparam Cartesian xy point type
        \tparam Parameters parameter type
        \par Projection characteristics
         - Azimuthal
         - Spheroid
         - Ellipsoid
         - lat_0 guam
        \par Example
        \image html ex_aeqd.gif
    */
    template <typename Geographic, typename Cartesian, typename Parameters = parameters>
    struct aeqd_guam : public detail::aeqd::base_aeqd_guam<Geographic, Cartesian, Parameters>
    {
        inline aeqd_guam(const Parameters& par) : detail::aeqd::base_aeqd_guam<Geographic, Cartesian, Parameters>(par)
        {
            detail::aeqd::setup_aeqd(this->m_par, this->m_proj_parm);
        }
    };

    /*!
        \brief Azimuthal Equidistant projection
        \ingroup projections
        \tparam Geographic latlong point type
        \tparam Cartesian xy point type
        \tparam Parameters parameter type
        \par Projection characteristics
         - Azimuthal
         - Spheroid
         - Ellipsoid
         - lat_0 guam
        \par Example
        \image html ex_aeqd.gif
    */
    template <typename Geographic, typename Cartesian, typename Parameters = parameters>
    struct aeqd_spheroid : public detail::aeqd::base_aeqd_spheroid<Geographic, Cartesian, Parameters>
    {
        inline aeqd_spheroid(const Parameters& par) : detail::aeqd::base_aeqd_spheroid<Geographic, Cartesian, Parameters>(par)
        {
            detail::aeqd::setup_aeqd(this->m_par, this->m_proj_parm);
        }
    };

    #ifndef DOXYGEN_NO_DETAIL
    namespace detail
    {

        // Factory entry(s)
        template <typename Geographic, typename Cartesian, typename Parameters>
        class aeqd_entry : public detail::factory_entry<Geographic, Cartesian, Parameters>
        {
            public :
                virtual projection<Geographic, Cartesian>* create_new(const Parameters& par) const
                {
                    if (! par.es)
                        return new base_v_fi<aeqd_spheroid<Geographic, Cartesian, Parameters>, Geographic, Cartesian, Parameters>(par);
                    else if (pj_param(par.params, "bguam").i)
                        return new base_v_fi<aeqd_guam<Geographic, Cartesian, Parameters>, Geographic, Cartesian, Parameters>(par);
                    else
                        return new base_v_fi<aeqd_ellipsoid<Geographic, Cartesian, Parameters>, Geographic, Cartesian, Parameters>(par);
                }
        };

        template <typename Geographic, typename Cartesian, typename Parameters>
        inline void aeqd_init(detail::base_factory<Geographic, Cartesian, Parameters>& factory)
        {
            factory.add_to_factory("aeqd", new aeqd_entry<Geographic, Cartesian, Parameters>);
        }

    } // namespace detail
    #endif // doxygen

}}} // namespace boost::geometry::projections

#endif // BOOST_GEOMETRY_PROJECTIONS_AEQD_HPP
