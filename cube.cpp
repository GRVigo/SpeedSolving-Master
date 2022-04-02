/*  This file is part of "GR Cube"

    Copyright (C) 2021 German Ramos Rodriguez

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    German Ramos Rodriguez
    Vigo, Spain
    grvigo@hotmail.com
*/

#include "cube.h"

namespace grcube3
{
    const std::array<Lyr, 6u> Cube::ExtLayers = { Lyr::U, Lyr::D, Lyr::F, Lyr::B, Lyr::R, Lyr::L }; // External layers
	
    // Arrays to follow spins when a cube has a turn

    const Spn Cube::spn_Turns_x[24] =
    {
        Spn::FD ,Spn::RD, Spn::BD, Spn::LD,
        Spn::FU, Spn::RU, Spn::BU, Spn::LU,
        Spn::UB, Spn::RB, Spn::DB, Spn::LB,
        Spn::UF, Spn::RF, Spn::DF, Spn::LF,
        Spn::UL, Spn::FL, Spn::DL, Spn::BL,
        Spn::UR, Spn::FR, Spn::DR, Spn::BR
    };

    const Spn Cube::spn_Turns_xp[24] =
    {
        Spn::BU, Spn::LU, Spn::FU, Spn::RU,
        Spn::BD, Spn::LD, Spn::FD, Spn::RD,
        Spn::DF, Spn::LF, Spn::UF, Spn::RF,
        Spn::DB, Spn::LB, Spn::UB, Spn::RB,
        Spn::DR, Spn::BR, Spn::UR, Spn::FR,
        Spn::DL, Spn::BL, Spn::UL, Spn::FL
    };

    const Spn Cube::spn_Turns_x2[24] =
    {
        Spn::DB, Spn::DL, Spn::DF, Spn::DR,
        Spn::UB, Spn::UL, Spn::UF, Spn::UR,
        Spn::BD, Spn::BL, Spn::BU, Spn::BR,
        Spn::FD, Spn::FL, Spn::FU, Spn::FR,
        Spn::LD, Spn::LB, Spn::LU, Spn::LF,
        Spn::RD, Spn::RB, Spn::RU, Spn::RF
    };

    const Spn Cube::spn_Turns_y[24] =
    {
        Spn::UR, Spn::UB, Spn::UL, Spn::UF,
        Spn::DL, Spn::DF, Spn::DR, Spn::DB,
        Spn::FL, Spn::FU, Spn::FR, Spn::FD,
        Spn::BR, Spn::BD, Spn::BL, Spn::BU,
        Spn::RF, Spn::RD, Spn::RB, Spn::RU,
        Spn::LB, Spn::LU, Spn::LF, Spn::LD
    };

    const Spn Cube::spn_Turns_yp[24] =
    {
        Spn::UL, Spn::UF, Spn::UR, Spn::UB,
        Spn::DR, Spn::DB, Spn::DL, Spn::DF,
        Spn::FR, Spn::FD, Spn::FL, Spn::FU,
        Spn::BL, Spn::BU, Spn::BR, Spn::BD,
        Spn::RB, Spn::RU, Spn::RF, Spn::RD,
        Spn::LF, Spn::LD, Spn::LB, Spn::LU
    };

    const Spn Cube::spn_Turns_y2[24] =
    {
        Spn::UB, Spn::UL, Spn::UF, Spn::UR,
        Spn::DB, Spn::DL, Spn::DF, Spn::DR,
        Spn::FD, Spn::FL, Spn::FU, Spn::FR,
        Spn::BD, Spn::BL, Spn::BU, Spn::BR,
        Spn::RD, Spn::RB, Spn::RU, Spn::RF,
        Spn::LD, Spn::LB, Spn::LU, Spn::LF,
    };

    const Spn Cube::spn_Turns_z[24] =
    {
        Spn::LF, Spn::FR, Spn::RB, Spn::BL,
        Spn::RF, Spn::BR, Spn::LB, Spn::FL,
        Spn::RU, Spn::DR, Spn::LD, Spn::UL,
        Spn::LU, Spn::UR, Spn::RD, Spn::DL,
        Spn::BU, Spn::UF, Spn::FD, Spn::DB,
        Spn::FU, Spn::DF, Spn::BD, Spn::UB
    };

    const Spn Cube::spn_Turns_zp[24] =
    {
        Spn::RF, Spn::BR, Spn::LB, Spn::FL,
        Spn::LF, Spn::FR, Spn::RB, Spn::BL,
        Spn::LU, Spn::UR, Spn::RD, Spn::DL,
        Spn::RU, Spn::DR, Spn::LD, Spn::UL,
        Spn::FU, Spn::DF, Spn::BD, Spn::UB,
        Spn::BU, Spn::UF, Spn::FD, Spn::DB
    };

    const Spn Cube::spn_Turns_z2[24] =
    {
        Spn::DF, Spn::DR, Spn::DB, Spn::DL,
        Spn::UF, Spn::UR, Spn::UB, Spn::UL,
        Spn::BU, Spn::BR, Spn::BD, Spn::BL,
        Spn::FU, Spn::FR, Spn::FD, Spn::FL,
        Spn::LU, Spn::LF, Spn::LD, Spn::LB,
        Spn::RU, Spn::RF, Spn::RD, Spn::RB
    };

    // Two turns array for get a final spin from an inital spin [Start spin][Final spin][Turns to pass from start to final spin]
    const Stp Cube::stp_Turns[24][24][2] =
    { //            UF                      UR                      UB                       UL                       DF                       DR                      DB                       DL                       FU                       FR                       FD                        FL                       BU                       BR                       BD                       BL                       RU                       RF                       RD                       RB                       LU                       LF                       LD                       LB
        { { Stp::NONE, Stp::NONE}, { Stp::y, Stp::NONE},    { Stp::y2, Stp::NONE},   { Stp::yp, Stp::NONE},   { Stp::z2, Stp::NONE},   { Stp::z2, Stp::yp },    { Stp::x2, Stp::NONE},   { Stp::z2, Stp::y },     { Stp::z2, Stp::x },     { Stp::z, Stp::x },      { Stp::x, Stp::NONE},    { Stp::zp, Stp::x },     { Stp::xp, Stp::NONE},   { Stp::zp, Stp::xp },    { Stp::z2, Stp::xp },    { Stp::z, Stp::xp },     { Stp::zp, Stp::yp },    { Stp::zp, Stp::NONE},   { Stp::zp, Stp::y },     { Stp::zp, Stp::y2 },    { Stp::z, Stp::y },      { Stp::z, Stp::NONE},    { Stp::z, Stp::yp },     { Stp::zp, Stp::x2 }    }, // UF
        { { Stp::yp, Stp::NONE},   { Stp::NONE, Stp::NONE}, { Stp::y, Stp::NONE},    { Stp::y2, Stp::NONE},   { Stp::z2, Stp::y },     { Stp::z2, Stp::NONE},   { Stp::z2, Stp::yp },    { Stp::x2, Stp::NONE},   { Stp::z, Stp::y },      { Stp::z, Stp::NONE},    { Stp::z, Stp::yp },     { Stp::zp, Stp::x2 },    { Stp::zp, Stp::yp },    { Stp::zp, Stp::NONE},   { Stp::zp, Stp::y },     { Stp::zp, Stp::y2 },    { Stp::z2, Stp::x },     { Stp::zp, Stp::x },     { Stp::x, Stp::NONE},    { Stp::z, Stp::x },      { Stp::xp, Stp::NONE},   { Stp::z, Stp::xp },     { Stp::z2, Stp::xp },    { Stp::zp, Stp::xp }    }, // UR
        { { Stp::y2, Stp::NONE},   { Stp::yp, Stp::NONE},   { Stp::NONE, Stp::NONE}, { Stp::y, Stp::NONE},    { Stp::x2, Stp::NONE},   { Stp::z2, Stp::y },     { Stp::z2, Stp::NONE},   { Stp::z2, Stp::yp },    { Stp::xp, Stp::NONE},   { Stp::z, Stp::xp },     { Stp::z2, Stp::xp },    { Stp::zp, Stp::xp },    { Stp::z2, Stp::x },     { Stp::zp, Stp::x },     { Stp::x, Stp::NONE},    { Stp::z, Stp::x },      { Stp::z, Stp::y },      { Stp::zp, Stp::x2 },    { Stp::z, Stp::yp },     { Stp::z, Stp::NONE},    { Stp::zp, Stp::yp },    { Stp::zp, Stp::y2 },    { Stp::zp, Stp::y },     { Stp::zp, Stp::NONE}   }, // UB
        { { Stp::y, Stp::NONE},    { Stp::y2, Stp::NONE},   { Stp::yp, Stp::NONE},   { Stp::NONE, Stp::NONE}, { Stp::z2, Stp::yp },    { Stp::x2, Stp::NONE},   { Stp::z2, Stp::y },     { Stp::z2, Stp::NONE},   { Stp::zp, Stp::yp },    { Stp::zp, Stp::y2 },    { Stp::zp, Stp::y },     { Stp::zp, Stp::NONE},   { Stp::z, Stp::y },      { Stp::zp, Stp::x2 },    { Stp::z, Stp::yp },     { Stp::z, Stp::NONE},    { Stp::xp, Stp::NONE},   { Stp::zp, Stp::xp },    { Stp::z2, Stp::xp },    { Stp::z, Stp::xp },     { Stp::z2, Stp::x },     { Stp::z, Stp::x },      { Stp::x, Stp::NONE},    { Stp::zp, Stp::x }     }, // UL
        { { Stp::z2, Stp::NONE},   { Stp::z2, Stp::y },     { Stp::x2, Stp::NONE},   { Stp::z2, Stp::yp },    { Stp::NONE, Stp::NONE}, { Stp::yp, Stp::NONE},   { Stp::y2, Stp::NONE},   { Stp::y, Stp::NONE},    { Stp::x, Stp::NONE},    { Stp::zp, Stp::x },     { Stp::z2, Stp::x },     { Stp::z, Stp::x },      { Stp::z2, Stp::xp },    { Stp::z, Stp::xp },     { Stp::xp, Stp::NONE},   { Stp::zp, Stp::xp },    { Stp::z, Stp::yp },     { Stp::z, Stp::NONE},    { Stp::z, Stp::y },      { Stp::zp, Stp::x2 },    { Stp::zp, Stp::y },     { Stp::zp, Stp::NONE},   { Stp::zp, Stp::yp },    { Stp::zp, Stp::y2 }    }, // DF
        { { Stp::z2, Stp::yp },    { Stp::z2, Stp::NONE},   { Stp::z2, Stp::y },     { Stp::x2, Stp::NONE},   { Stp::y, Stp::NONE},    { Stp::NONE, Stp::NONE}, { Stp::yp, Stp::NONE},   { Stp::y2, Stp::NONE},   { Stp::zp, Stp::y },     { Stp::zp, Stp::NONE},   { Stp::zp, Stp::yp },    { Stp::zp, Stp::y2 },    { Stp::z, Stp::yp },     { Stp::z, Stp::NONE},    { Stp::z, Stp::y },      { Stp::zp, Stp::x2 },    { Stp::x, Stp::NONE},    { Stp::z, Stp::x },      { Stp::z2, Stp::x },     { Stp::zp, Stp::x },     { Stp::z2, Stp::xp },    { Stp::zp, Stp::xp },    { Stp::xp, Stp::NONE},   { Stp::z, Stp::xp }     }, // DR
        { { Stp::x2, Stp::NONE},   { Stp::z2, Stp::yp },    { Stp::z2, Stp::NONE},   { Stp::z2, Stp::y },     { Stp::y2, Stp::NONE},   { Stp::y, Stp::NONE},    { Stp::NONE, Stp::NONE}, { Stp::yp, Stp::NONE},   { Stp::z2, Stp::xp },    { Stp::zp, Stp::xp },    { Stp::xp, Stp::NONE},   { Stp::z, Stp::xp },     { Stp::x, Stp::NONE},    { Stp::z, Stp::x },      { Stp::z2, Stp::x },     { Stp::zp, Stp::x },     { Stp::zp, Stp::y },     { Stp::zp, Stp::y2 },    { Stp::zp, Stp::yp },    { Stp::zp, Stp::NONE},   { Stp::z, Stp::yp },     { Stp::zp, Stp::x2 },    { Stp::z, Stp::y },      { Stp::z, Stp::NONE}    }, // DB
        { { Stp::z2, Stp::y },     { Stp::x2, Stp::NONE},   { Stp::z2, Stp::yp },    { Stp::z2, Stp::NONE},   { Stp::yp, Stp::NONE},   { Stp::y2, Stp::NONE},   { Stp::y, Stp::NONE},    { Stp::NONE, Stp::NONE}, { Stp::z, Stp::yp },     { Stp::zp, Stp::x2 },    { Stp::z, Stp::y },      { Stp::z, Stp::NONE},    { Stp::zp, Stp::y },     { Stp::zp, Stp::y2 },    { Stp::zp, Stp::yp },    { Stp::zp, Stp::NONE},   { Stp::z2, Stp::xp },    { Stp::z, Stp::xp },     { Stp::xp, Stp::NONE},   { Stp::zp, Stp::xp },    { Stp::x, Stp::NONE},    { Stp::zp, Stp::x },     { Stp::z2, Stp::x },     { Stp::z, Stp::x }      }, // DL
        { { Stp::z2, Stp::x },     { Stp::zp, Stp::x },     { Stp::x, Stp::NONE},    { Stp::z, Stp::x },      { Stp::xp, Stp::NONE},   { Stp::z, Stp::xp },     { Stp::z2, Stp::xp },    { Stp::zp, Stp::xp },    { Stp::NONE, Stp::NONE}, { Stp::yp, Stp::NONE},   { Stp::y2, Stp::NONE},   { Stp::y, Stp::NONE},    { Stp::z2, Stp::NONE},   { Stp::z2, Stp::y },     { Stp::x2, Stp::NONE},   { Stp::z2, Stp::yp },    { Stp::z, Stp::NONE},    { Stp::z, Stp::y },      { Stp::zp, Stp::x2 },    { Stp::z, Stp::yp },     { Stp::zp, Stp::NONE},   { Stp::zp, Stp::yp },    { Stp::zp, Stp::y2 },    { Stp::zp, Stp::y }     }, // FU
        { { Stp::zp, Stp::yp },    { Stp::zp, Stp::NONE},   { Stp::zp, Stp::y },     { Stp::zp, Stp::y2 },    { Stp::z, Stp::y },      { Stp::z, Stp::NONE},    { Stp::z, Stp::yp },     { Stp::zp, Stp::x2 },    { Stp::y, Stp::NONE},    { Stp::NONE, Stp::NONE}, { Stp::yp, Stp::NONE},   { Stp::y2, Stp::NONE},   { Stp::z2, Stp::yp },    { Stp::z2, Stp::NONE},   { Stp::z2, Stp::y },     { Stp::x2, Stp::NONE},   { Stp::z, Stp::x },      { Stp::z2, Stp::x },     { Stp::zp, Stp::x },     { Stp::x, Stp::NONE},    { Stp::zp, Stp::xp },    { Stp::xp, Stp::NONE},   { Stp::z, Stp::xp },     { Stp::z2, Stp::xp }    }, // FR
        { { Stp::xp, Stp::NONE},   { Stp::zp, Stp::xp },    { Stp::z2, Stp::xp },    { Stp::z, Stp::xp },     { Stp::z2, Stp::x },     { Stp::z, Stp::x },      { Stp::x, Stp::NONE},    { Stp::zp, Stp::x },     { Stp::y2, Stp::NONE},   { Stp::y, Stp::NONE},    { Stp::NONE, Stp::NONE}, { Stp::yp, Stp::NONE},   { Stp::x2, Stp::NONE},   { Stp::z2, Stp::yp },    { Stp::z2, Stp::NONE},   { Stp::z2, Stp::y },     { Stp::zp, Stp::y2 },    { Stp::zp, Stp::yp },    { Stp::zp, Stp::NONE},   { Stp::zp, Stp::y },     { Stp::zp, Stp::x2 },    { Stp::z, Stp::y },      { Stp::z, Stp::NONE},    { Stp::z, Stp::yp }     }, // FD
        { { Stp::z, Stp::y },      { Stp::zp, Stp::x2 },    { Stp::z, Stp::yp },     { Stp::z, Stp::NONE},    { Stp::zp, Stp::yp },    { Stp::zp, Stp::y2 },    { Stp::zp, Stp::y },     { Stp::zp, Stp::NONE},   { Stp::yp, Stp::NONE},   { Stp::y2, Stp::NONE},   { Stp::y, Stp::NONE},    { Stp::NONE, Stp::NONE}, { Stp::z2, Stp::y },     { Stp::x2, Stp::NONE},   { Stp::z2, Stp::yp },    { Stp::z2, Stp::NONE},   { Stp::z, Stp::xp },     { Stp::xp, Stp::NONE},   { Stp::zp, Stp::xp },    { Stp::z2, Stp::xp },    { Stp::zp, Stp::x },     { Stp::z2, Stp::x },     { Stp::z, Stp::x },      { Stp::x, Stp::NONE}    }, // FL
        { { Stp::x, Stp::NONE},    { Stp::z, Stp::x },      { Stp::z2, Stp::x },     { Stp::zp, Stp::x },     { Stp::z2, Stp::xp },    { Stp::zp, Stp::xp },    { Stp::xp, Stp::NONE},   { Stp::z, Stp::xp },     { Stp::z2, Stp::NONE},   { Stp::z2, Stp::yp },    { Stp::x2, Stp::NONE},   { Stp::z2, Stp::y },     { Stp::NONE, Stp::NONE}, { Stp::y, Stp::NONE},    { Stp::y2, Stp::NONE},   { Stp::yp, Stp::NONE},   { Stp::zp, Stp::NONE},   { Stp::zp, Stp::y },     { Stp::zp, Stp::y2 },    { Stp::zp, Stp::yp },    { Stp::z, Stp::NONE},    { Stp::z, Stp::yp },     { Stp::zp, Stp::x2 },    { Stp::z, Stp::y }      }, // BU
        { { Stp::z, Stp::yp },     { Stp::z, Stp::NONE},    { Stp::z, Stp::y },      { Stp::zp, Stp::x2 },    { Stp::zp, Stp::y },     { Stp::zp, Stp::NONE},   { Stp::zp, Stp::yp },    { Stp::zp, Stp::y2 },    { Stp::z2, Stp::y },     { Stp::z2, Stp::NONE},   { Stp::z2, Stp::yp },    { Stp::x2, Stp::NONE},   { Stp::yp, Stp::NONE},   { Stp::NONE, Stp::NONE}, { Stp::y, Stp::NONE},    { Stp::y2, Stp::NONE},   { Stp::zp, Stp::x },     { Stp::x, Stp::NONE},    { Stp::z, Stp::x },      { Stp::z2, Stp::x },     { Stp::z, Stp::xp },     { Stp::z2, Stp::xp },    { Stp::zp, Stp::xp },    { Stp::xp, Stp::NONE}   }, // BR
        { { Stp::z2, Stp::xp },    { Stp::z, Stp::xp },     { Stp::xp, Stp::NONE},   { Stp::zp, Stp::xp },    { Stp::x, Stp::NONE},    { Stp::zp, Stp::x },     { Stp::z2, Stp::x },     { Stp::z, Stp::x },      { Stp::x2, Stp::NONE},   { Stp::z2, Stp::y },     { Stp::z2, Stp::NONE},   { Stp::z2, Stp::yp },    { Stp::y2, Stp::NONE},   { Stp::yp, Stp::NONE},   { Stp::NONE, Stp::NONE}, { Stp::y, Stp::NONE},    { Stp::zp, Stp::x2 },    { Stp::z, Stp::yp },     { Stp::z, Stp::NONE},    { Stp::z, Stp::y },      { Stp::zp, Stp::y2 },    { Stp::zp, Stp::y },     { Stp::zp, Stp::NONE},   { Stp::zp, Stp::yp }    }, // BD
        { { Stp::zp, Stp::y },     { Stp::zp, Stp::y2 },    { Stp::zp, Stp::yp },    { Stp::zp, Stp::NONE},   { Stp::z, Stp::yp },     { Stp::zp, Stp::x2 },    { Stp::z, Stp::y },      { Stp::z, Stp::NONE},    { Stp::z2, Stp::yp },    { Stp::x2, Stp::NONE},   { Stp::z2, Stp::y },     { Stp::z2, Stp::NONE},   { Stp::y, Stp::NONE},    { Stp::y2, Stp::NONE},   { Stp::yp, Stp::NONE},   { Stp::NONE, Stp::NONE}, { Stp::zp, Stp::xp },    { Stp::z2, Stp::xp },    { Stp::z, Stp::xp },     { Stp::xp, Stp::NONE},   { Stp::z, Stp::x },      { Stp::x, Stp::NONE},    { Stp::zp, Stp::x },     { Stp::z2, Stp::x }     }, // BL
        { { Stp::z, Stp::x },      { Stp::z2, Stp::x },     { Stp::zp, Stp::x },     { Stp::x, Stp::NONE},    { Stp::zp, Stp::xp },    { Stp::xp, Stp::NONE},   { Stp::z, Stp::xp },     { Stp::z2, Stp::xp },    { Stp::zp, Stp::NONE},   { Stp::zp, Stp::yp },    { Stp::zp, Stp::y2 },    { Stp::zp, Stp::y },     { Stp::z, Stp::NONE},    { Stp::z, Stp::y },      { Stp::zp, Stp::x2 },    { Stp::z, Stp::yp },     { Stp::NONE, Stp::NONE}, { Stp::y, Stp::NONE},    { Stp::y2, Stp::NONE},   { Stp::yp, Stp::NONE},   { Stp::z2, Stp::NONE},   { Stp::z2, Stp::yp },    { Stp::x2, Stp::NONE},   { Stp::z2, Stp::y }     }, // RU
        { { Stp::z, Stp::NONE},    { Stp::z, Stp::y },      { Stp::zp, Stp::x2 },    { Stp::z, Stp::yp },     { Stp::zp, Stp::NONE},   { Stp::zp, Stp::yp },    { Stp::zp, Stp::y2 },    { Stp::zp, Stp::y },     { Stp::zp, Stp::x },     { Stp::z2, Stp::x },     { Stp::z, Stp::x },      { Stp::x, Stp::NONE},    { Stp::z, Stp::xp },     { Stp::xp, Stp::NONE},   { Stp::zp, Stp::xp },    { Stp::z2, Stp::xp },    { Stp::yp, Stp::NONE},   { Stp::NONE, Stp::NONE}, { Stp::y, Stp::NONE},    { Stp::y2, Stp::NONE},   { Stp::z2, Stp::y },     { Stp::z2, Stp::NONE},   { Stp::z2, Stp::yp },    { Stp::x2, Stp::NONE}   }, // RF
        { { Stp::z, Stp::xp },     { Stp::xp, Stp::NONE},   { Stp::zp, Stp::xp },    { Stp::z2, Stp::xp },    { Stp::zp, Stp::x },     { Stp::z2, Stp::x },     { Stp::z, Stp::x },      { Stp::x, Stp::NONE},    { Stp::zp, Stp::x2 },    { Stp::z, Stp::y },      { Stp::z, Stp::NONE},    { Stp::z, Stp::yp },     { Stp::zp, Stp::y2 },    { Stp::zp, Stp::yp },    { Stp::zp, Stp::NONE},   { Stp::zp, Stp::y },     { Stp::y2, Stp::NONE},   { Stp::yp, Stp::NONE},   { Stp::NONE, Stp::NONE}, { Stp::y, Stp::NONE},    { Stp::x2, Stp::NONE},   { Stp::z2, Stp::y },     { Stp::z2, Stp::NONE},   { Stp::z2, Stp::yp }    }, // RD
        { { Stp::zp, Stp::y2 },    { Stp::zp, Stp::yp },    { Stp::zp, Stp::NONE},   { Stp::zp, Stp::y },     { Stp::zp, Stp::x2 },    { Stp::z, Stp::y },      { Stp::z, Stp::NONE},    { Stp::z, Stp::yp },     { Stp::zp, Stp::xp },    { Stp::xp, Stp::NONE},   { Stp::z, Stp::xp },     { Stp::z2, Stp::xp },    { Stp::z, Stp::x },      { Stp::z2, Stp::x },     { Stp::zp, Stp::x },     { Stp::x, Stp::NONE},    { Stp::y, Stp::NONE},    { Stp::y2, Stp::NONE},   { Stp::yp, Stp::NONE},   { Stp::NONE, Stp::NONE}, { Stp::z2, Stp::yp },    { Stp::x2, Stp::NONE},   { Stp::z2, Stp::y },     { Stp::z2, Stp::NONE}   }, // RB
        { { Stp::zp, Stp::x },     { Stp::x, Stp::NONE},    { Stp::z, Stp::x },      { Stp::z2, Stp::x },     { Stp::z, Stp::xp },     { Stp::z2, Stp::xp },    { Stp::zp, Stp::xp },    { Stp::xp, Stp::NONE},   { Stp::z, Stp::NONE},    { Stp::z, Stp::yp },     { Stp::zp, Stp::x2 },    { Stp::z, Stp::y },      { Stp::zp, Stp::NONE},   { Stp::zp, Stp::y },     { Stp::zp, Stp::y2 },    { Stp::zp, Stp::yp },    { Stp::z2, Stp::NONE},   { Stp::z2, Stp::y },     { Stp::x2, Stp::NONE},   { Stp::z2, Stp::yp },    { Stp::NONE, Stp::NONE}, { Stp::yp, Stp::NONE},   { Stp::y2, Stp::NONE},   { Stp::y, Stp::NONE}    }, // LU
        { { Stp::zp, Stp::NONE},   { Stp::zp, Stp::y },     { Stp::zp, Stp::y2 },    { Stp::zp, Stp::yp },    { Stp::z, Stp::NONE},    { Stp::z, Stp::yp },     { Stp::zp, Stp::x2 },    { Stp::z, Stp::y },      { Stp::z, Stp::x },      { Stp::x, Stp::NONE},    { Stp::zp, Stp::x },     { Stp::z2, Stp::x },     { Stp::zp, Stp::xp },    { Stp::z2, Stp::xp },    { Stp::z, Stp::xp },     { Stp::xp, Stp::NONE},   { Stp::z2, Stp::yp },    { Stp::z2, Stp::NONE},   { Stp::z2, Stp::y },     { Stp::x2, Stp::NONE},   { Stp::y, Stp::NONE},    { Stp::NONE, Stp::NONE}, { Stp::yp, Stp::NONE},   { Stp::y2, Stp::NONE}   }, // LF
        { { Stp::zp, Stp::xp },    { Stp::z2, Stp::xp },    { Stp::z, Stp::xp },     { Stp::xp, Stp::NONE},   { Stp::z, Stp::x },      { Stp::x, Stp::NONE},    { Stp::zp, Stp::x },     { Stp::z2, Stp::x },     { Stp::zp, Stp::y2 },    { Stp::zp, Stp::y },     { Stp::zp, Stp::NONE},   { Stp::zp, Stp::yp },    { Stp::zp, Stp::x2 },    { Stp::z, Stp::yp },     { Stp::z, Stp::NONE},    { Stp::z, Stp::y },      { Stp::x2, Stp::NONE},   { Stp::z2, Stp::yp },    { Stp::z2, Stp::NONE},   { Stp::z2, Stp::y },     { Stp::y2, Stp::NONE},   { Stp::y, Stp::NONE},    { Stp::NONE, Stp::NONE}, { Stp::yp, Stp::NONE}   }, // LD
        { { Stp::zp, Stp::x2 },    { Stp::z, Stp::yp },     { Stp::z, Stp::NONE},    { Stp::z, Stp::y },      { Stp::zp, Stp::y2 },    { Stp::zp, Stp::y },     { Stp::zp, Stp::NONE},   { Stp::zp, Stp::yp },    { Stp::z, Stp::xp },     { Stp::z2, Stp::xp },    { Stp::zp, Stp::xp },    { Stp::xp, Stp::NONE},   { Stp::zp, Stp::x },     { Stp::x, Stp::NONE},    { Stp::z, Stp::x },      { Stp::z2, Stp::x },     { Stp::z2, Stp::y },     { Stp::x2, Stp::NONE},   { Stp::z2, Stp::yp },    { Stp::z2, Stp::NONE},   { Stp::yp, Stp::NONE},   { Stp::y2, Stp::NONE},   { Stp::y, Stp::NONE},    { Stp::NONE, Stp::NONE} }  // LB
    };
    
    const Fce Cube::fce_Stickers[54] = // Array to know the solved face of the stickers
    {
        // Centers from 0 to 5
        Fce::U, Fce::D, Fce::F, Fce::B, Fce::R, Fce::L,  // Centers
        // Edges from 6 to 29
        Fce::F, Fce::R, Fce::F, Fce::L, Fce::B, Fce::L, Fce::B, Fce::R, // Middle layer
        Fce::U, Fce::F, Fce::U, Fce::L, Fce::U, Fce::B, Fce::U, Fce::R, // Upper layer
        Fce::D, Fce::F, Fce::D, Fce::L, Fce::D, Fce::B, Fce::D, Fce::R, // Lower layer
        // Corners from 30 to 53
        Fce::U, Fce::F, Fce::R, Fce::U, Fce::F, Fce::L, // Upper layer
        Fce::U, Fce::B, Fce::L, Fce::U, Fce::B, Fce::R, // Upper layer
        Fce::D, Fce::F, Fce::R, Fce::D, Fce::F, Fce::L, // Lower layer
        Fce::D, Fce::B, Fce::L, Fce::D, Fce::B, Fce::R  // Lower layer
    };

	const Pce Cube::pce_Stickers[54] // Array to know the piece of the stickers
	{
		Pce::U, Pce::D, Pce::F, Pce::B, Pce::R, Pce::L,  // Centers
		// Edges
		Pce::FR, Pce::FR, Pce::FL, Pce::FL, Pce::BL, Pce::BL, Pce::BR, Pce::BR, // Middle layer
		Pce::UF, Pce::UF, Pce::UL, Pce::UL, Pce::UB, Pce::UB, Pce::UR, Pce::UR, // Upper layer
		Pce::DF, Pce::DF, Pce::DL, Pce::DL, Pce::DB, Pce::DB, Pce::DR, Pce::DR, // Lower layer
		// Corners
		Pce::UFR, Pce::UFR, Pce::UFR, Pce::UFL, Pce::UFL, Pce::UFL, // Upper layer
		Pce::UBL, Pce::UBL, Pce::UBL, Pce::UBR, Pce::UBR, Pce::UBR, // Upper layer
		Pce::DFR, Pce::DFR, Pce::DFR, Pce::DFL, Pce::DFL, Pce::DFL, // Lower layer
		Pce::DBL, Pce::DBL, Pce::DBL, Pce::DBR, Pce::DBR, Pce::DBR  // Lower layer
	};
	
    const Pct Cube::pct_Pieces[26] = // Array to know the pieces type
    {
        Pct::CENTER, Pct::CENTER, Pct::CENTER, Pct::CENTER, Pct::CENTER, Pct::CENTER, 
        Pct::EDGE, Pct::EDGE, Pct::EDGE, Pct::EDGE, Pct::EDGE, Pct::EDGE, 
        Pct::EDGE, Pct::EDGE, Pct::EDGE, Pct::EDGE, Pct::EDGE, Pct::EDGE, 
        Pct::CORNER, Pct::CORNER, Pct::CORNER, Pct::CORNER,
        Pct::CORNER, Pct::CORNER, Pct::CORNER, Pct::CORNER
    };

    const Lyr Cube::lyr_OppositeLayers[10] = // Array to know the opposite layer
    {
        Lyr::NONE, Lyr::D, Lyr::U, Lyr::B, Lyr::F, Lyr::L, Lyr::R, Lyr::E, Lyr::S, Lyr::M
    };

    const Lyr Cube::lyr_AdjacentLayers[10] = // Array to know the inner adjacent layer to an external layer
    {
        Lyr::NONE, Lyr::E, Lyr::E, Lyr::S, Lyr::S, Lyr::M, Lyr::M, Lyr::NONE, Lyr::NONE, Lyr::NONE
    };

    const Fce Cube::fce_OppositeFaces[6] = // Array to know the opposite face
    {
        Fce::D, Fce::U, Fce::B, Fce::F, Fce::L, Fce::R
    };

    const char Cube::char_Layers[10] = // Array to know the layer char
    {
        '-', 'U', 'D', 'F', 'B', 'R', 'L', 'E', 'S', 'M'
    };

    // Array with spins string representation
    const std::string Cube::string_Spins[24] =
    {
        "UF", "UR", "UB", "UL",
        "DF", "DR", "DB", "DL",
        "FU", "FR", "FD", "FL",
        "BU", "BR", "BD", "BL",
        "RU", "RF", "RD", "RB",
        "LU", "LF", "LD", "LB"
    };

    // Array with pieces string representation
    const std::string Cube::string_Pieces[26] =
    {
        "U", "D", "F", "B", "R", "L",
		"FR", "FL", "BL", "BR", "UF", "UL", "UB", "UR", "DF", "DL", "DB", "DR",
		"UFR", "UFL", "UBL", "UBR", "DFR", "DFL", "DBL", "DBR"
    };

    // Array with stickers string representation
    const std::string Cube::string_Stickers[54] =
    {
        "U", "D", "F", "B", "R", "L",
		"FR_F", "FR_R", "FL_F", "FL_L", "BL_B", "BL_L", "BR_B", "BR_R",
        "UF_U", "UF_F", "UL_U", "UL_L", "UB_U", "UB_B", "UR_U", "UR_R",
        "DF_D", "DF_F", "DL_D", "DL_L", "DB_D", "DB_B", "DR_D", "DR_R",
		"UFR_U", "UFR_F", "UFR_R", "UFL_U", "UFL_F", "UFL_L", "UBL_U", "UBL_B", "UBL_L", "UBR_U", "UBR_B", "UBR_R",
        "DFR_D", "DFR_F", "DFR_R", "DFL_D", "DFL_F", "DFL_L", "DBL_D", "DBL_B", "DBL_L", "DBR_D", "DBR_B", "DBR_R"
    };

    // Strings with all possible directions for pieces
    // Directions for edges:
	// 'E' layer edge in layer 'E': -EDGE_POS- if F/B sticker is in FB direction / -EDGE_NEG- if F/B sticker is in RL direction
	// 'E' layer edge in layer 'U/D': -EDGE_POS- if F/B sticker is in UD direction / -EDGE_NEG- if F/B sticker is in FB/RL directions
	// 'U/D' layer edge in layer 'E': -EDGE_POS- if U/D sticker is in FB direction / -EDGE_NEG- if U/D sticker is in RL direction
	// 'U/D' layer edge in layer 'U/D': -EDGE_POS- if U/D sticker is in UD direction / -EDGE_NEG- if U/D sticker is in FB/RL directions
	// Directions for corners:
	// U/D sticker direction: UD -> CORNER_UD / FB -> CORNER_FB / RL -> CORNER_RL
	const std::string Cube::string_Directions[6] =
	{
		"NONE", // Centers don't need direction
		"POS", "NEG", // Edg direction (positive / negative)
		"UD", "FB", "RL" // Cnr direction (UD, FB or RL direction for U/D sticker)
	};

    // Array to get the layer from spin and slice
    const Lyr Cube::lyr_Slice[24][10] =
    {
        { Lyr::NONE, Lyr::U, Lyr::D, Lyr::F, Lyr::B, Lyr::R, Lyr::L, Lyr::E, Lyr::S, Lyr::M }, // UF
        { Lyr::NONE, Lyr::U, Lyr::D, Lyr::R, Lyr::L, Lyr::B, Lyr::F, Lyr::E, Lyr::M, Lyr::S }, // UR
        { Lyr::NONE, Lyr::U, Lyr::D, Lyr::B, Lyr::F, Lyr::L, Lyr::R, Lyr::E, Lyr::S, Lyr::M }, // UB
        { Lyr::NONE, Lyr::U, Lyr::D, Lyr::L, Lyr::R, Lyr::F, Lyr::B, Lyr::E, Lyr::M, Lyr::S }, // UL
        { Lyr::NONE, Lyr::D, Lyr::U, Lyr::F, Lyr::B, Lyr::L, Lyr::R, Lyr::E, Lyr::S, Lyr::M }, // DF
        { Lyr::NONE, Lyr::D, Lyr::U, Lyr::R, Lyr::L, Lyr::F, Lyr::B, Lyr::E, Lyr::M, Lyr::S }, // DR
        { Lyr::NONE, Lyr::D, Lyr::U, Lyr::B, Lyr::F, Lyr::R, Lyr::L, Lyr::E, Lyr::S, Lyr::M }, // DB
        { Lyr::NONE, Lyr::D, Lyr::U, Lyr::L, Lyr::R, Lyr::B, Lyr::F, Lyr::E, Lyr::M, Lyr::S }, // DL
        { Lyr::NONE, Lyr::F, Lyr::B, Lyr::U, Lyr::D, Lyr::L, Lyr::R, Lyr::S, Lyr::E, Lyr::M }, // FU
        { Lyr::NONE, Lyr::F, Lyr::B, Lyr::R, Lyr::L, Lyr::U, Lyr::D, Lyr::S, Lyr::M, Lyr::E }, // FR
        { Lyr::NONE, Lyr::F, Lyr::B, Lyr::D, Lyr::U, Lyr::R, Lyr::L, Lyr::S, Lyr::E, Lyr::M }, // FD
        { Lyr::NONE, Lyr::F, Lyr::B, Lyr::L, Lyr::R, Lyr::D, Lyr::U, Lyr::S, Lyr::M, Lyr::E }, // FL
        { Lyr::NONE, Lyr::B, Lyr::F, Lyr::U, Lyr::D, Lyr::R, Lyr::L, Lyr::S, Lyr::E, Lyr::M }, // BU
        { Lyr::NONE, Lyr::B, Lyr::F, Lyr::R, Lyr::L, Lyr::D, Lyr::U, Lyr::S, Lyr::M, Lyr::E }, // BR
        { Lyr::NONE, Lyr::B, Lyr::F, Lyr::D, Lyr::U, Lyr::L, Lyr::R, Lyr::S, Lyr::E, Lyr::M }, // BD
        { Lyr::NONE, Lyr::B, Lyr::F, Lyr::L, Lyr::R, Lyr::U, Lyr::D, Lyr::S, Lyr::M, Lyr::E }, // BL
        { Lyr::NONE, Lyr::R, Lyr::L, Lyr::U, Lyr::D, Lyr::F, Lyr::B, Lyr::M, Lyr::E, Lyr::S }, // RU
        { Lyr::NONE, Lyr::R, Lyr::L, Lyr::F, Lyr::B, Lyr::D, Lyr::U, Lyr::M, Lyr::S, Lyr::E }, // RF
        { Lyr::NONE, Lyr::R, Lyr::L, Lyr::D, Lyr::U, Lyr::B, Lyr::F, Lyr::M, Lyr::E, Lyr::S }, // RD
        { Lyr::NONE, Lyr::R, Lyr::L, Lyr::B, Lyr::F, Lyr::U, Lyr::D, Lyr::M, Lyr::S, Lyr::E }, // RB
        { Lyr::NONE, Lyr::L, Lyr::R, Lyr::U, Lyr::D, Lyr::B, Lyr::F, Lyr::M, Lyr::E, Lyr::S }, // LU
        { Lyr::NONE, Lyr::L, Lyr::R, Lyr::F, Lyr::B, Lyr::U, Lyr::D, Lyr::M, Lyr::S, Lyr::E }, // LF
        { Lyr::NONE, Lyr::L, Lyr::R, Lyr::D, Lyr::U, Lyr::F, Lyr::B, Lyr::M, Lyr::E, Lyr::S }, // LD
        { Lyr::NONE, Lyr::L, Lyr::R, Lyr::B, Lyr::F, Lyr::D, Lyr::U, Lyr::M, Lyr::S, Lyr::E }  // LB
    };

    // Piece positions from spin and absolut piece positions
    const Pcp Cube::pcp_Absolut[24][26] =
    {
        { Pcp::U, Pcp::D, Pcp::F, Pcp::B, Pcp::R, Pcp::L, Pcp::FR, Pcp::FL, Pcp::BL, Pcp::BR, Pcp::UF, Pcp::UL, Pcp::UB, Pcp::UR, Pcp::DF, Pcp::DL, Pcp::DB, Pcp::DR, Pcp::UFR, Pcp::UFL, Pcp::UBL, Pcp::UBR, Pcp::DFR, Pcp::DFL, Pcp::DBL, Pcp::DBR }, // UF
        { Pcp::U, Pcp::D, Pcp::R, Pcp::L, Pcp::B, Pcp::F, Pcp::BR, Pcp::FR, Pcp::FL, Pcp::BL, Pcp::UR, Pcp::UF, Pcp::UL, Pcp::UB, Pcp::DR, Pcp::DF, Pcp::DL, Pcp::DB, Pcp::UBR, Pcp::UFR, Pcp::UFL, Pcp::UBL, Pcp::DBR, Pcp::DFR, Pcp::DFL, Pcp::DBL }, // UR
        { Pcp::U, Pcp::D, Pcp::B, Pcp::F, Pcp::L, Pcp::R, Pcp::BL, Pcp::BR, Pcp::FR, Pcp::FL, Pcp::UB, Pcp::UR, Pcp::UF, Pcp::UL, Pcp::DB, Pcp::DR, Pcp::DF, Pcp::DL, Pcp::UBL, Pcp::UBR, Pcp::UFR, Pcp::UFL, Pcp::DBL, Pcp::DBR, Pcp::DFR, Pcp::DFL }, // UB
        { Pcp::U, Pcp::D, Pcp::L, Pcp::R, Pcp::F, Pcp::B, Pcp::FL, Pcp::BL, Pcp::BR, Pcp::FR, Pcp::UL, Pcp::UB, Pcp::UR, Pcp::UF, Pcp::DL, Pcp::DB, Pcp::DR, Pcp::DF, Pcp::UFL, Pcp::UBL, Pcp::UBR, Pcp::UFR, Pcp::DFL, Pcp::DBL, Pcp::DBR, Pcp::DFR }, // UL
        { Pcp::D, Pcp::U, Pcp::F, Pcp::B, Pcp::L, Pcp::R, Pcp::FL, Pcp::FR, Pcp::BR, Pcp::BL, Pcp::DF, Pcp::DR, Pcp::DB, Pcp::DL, Pcp::UF, Pcp::UR, Pcp::UB, Pcp::UL, Pcp::DFL, Pcp::DFR, Pcp::DBR, Pcp::DBL, Pcp::UFL, Pcp::UFR, Pcp::UBR, Pcp::UBL }, // DF
        { Pcp::D, Pcp::U, Pcp::R, Pcp::L, Pcp::F, Pcp::B, Pcp::FR, Pcp::BR, Pcp::BL, Pcp::FL, Pcp::DR, Pcp::DB, Pcp::DL, Pcp::DF, Pcp::UR, Pcp::UB, Pcp::UL, Pcp::UF, Pcp::DFR, Pcp::DBR, Pcp::DBL, Pcp::DFL, Pcp::UFR, Pcp::UBR, Pcp::UBL, Pcp::UFL }, // DR
        { Pcp::D, Pcp::U, Pcp::B, Pcp::F, Pcp::R, Pcp::L, Pcp::BR, Pcp::BL, Pcp::FL, Pcp::FR, Pcp::DB, Pcp::DL, Pcp::DF, Pcp::DR, Pcp::UB, Pcp::UL, Pcp::UF, Pcp::UR, Pcp::DBR, Pcp::DBL, Pcp::DFL, Pcp::DFR, Pcp::UBR, Pcp::UBL, Pcp::UFL, Pcp::UFR }, // DB
        { Pcp::D, Pcp::U, Pcp::L, Pcp::R, Pcp::B, Pcp::F, Pcp::BL, Pcp::FL, Pcp::FR, Pcp::BR, Pcp::DL, Pcp::DF, Pcp::DR, Pcp::DB, Pcp::UL, Pcp::UF, Pcp::UR, Pcp::UB, Pcp::DBL, Pcp::DFL, Pcp::DFR, Pcp::DBR, Pcp::UBL, Pcp::UFL, Pcp::UFR, Pcp::UBR }, // DL
        { Pcp::F, Pcp::B, Pcp::U, Pcp::D, Pcp::L, Pcp::R, Pcp::UL, Pcp::UR, Pcp::DR, Pcp::DL, Pcp::UF, Pcp::FR, Pcp::DF, Pcp::FL, Pcp::UB, Pcp::BR, Pcp::DB, Pcp::BL, Pcp::UFL, Pcp::UFR, Pcp::DFR, Pcp::DFL, Pcp::UBL, Pcp::UBR, Pcp::DBR, Pcp::DBL }, // FU
        { Pcp::F, Pcp::B, Pcp::R, Pcp::L, Pcp::U, Pcp::D, Pcp::UR, Pcp::DR, Pcp::DL, Pcp::UL, Pcp::FR, Pcp::DF, Pcp::FL, Pcp::UF, Pcp::BR, Pcp::DB, Pcp::BL, Pcp::UB, Pcp::UFR, Pcp::DFR, Pcp::DFL, Pcp::UFL, Pcp::UBR, Pcp::DBR, Pcp::DBL, Pcp::UBL }, // FR
        { Pcp::F, Pcp::B, Pcp::D, Pcp::U, Pcp::R, Pcp::L, Pcp::DR, Pcp::DL, Pcp::UL, Pcp::UR, Pcp::DF, Pcp::FL, Pcp::UF, Pcp::FR, Pcp::DB, Pcp::BL, Pcp::UB, Pcp::BR, Pcp::DFR, Pcp::DFL, Pcp::UFL, Pcp::UFR, Pcp::DBR, Pcp::DBL, Pcp::UBL, Pcp::UBR }, // FD
        { Pcp::F, Pcp::B, Pcp::L, Pcp::R, Pcp::D, Pcp::U, Pcp::DL, Pcp::UL, Pcp::UR, Pcp::DR, Pcp::FL, Pcp::UF, Pcp::FR, Pcp::DF, Pcp::BL, Pcp::UB, Pcp::BR, Pcp::DB, Pcp::DFL, Pcp::UFL, Pcp::UFR, Pcp::DFR, Pcp::DBL, Pcp::UBL, Pcp::UBR, Pcp::DBR }, // FL
        { Pcp::B, Pcp::F, Pcp::U, Pcp::D, Pcp::R, Pcp::L, Pcp::UR, Pcp::UL, Pcp::DL, Pcp::DR, Pcp::UB, Pcp::BL, Pcp::DB, Pcp::BR, Pcp::UF, Pcp::FL, Pcp::DF, Pcp::FR, Pcp::UBR, Pcp::UBL, Pcp::DBL, Pcp::DBR, Pcp::UFR, Pcp::UFL, Pcp::DFL, Pcp::DFR }, // BU
        { Pcp::B, Pcp::F, Pcp::R, Pcp::L, Pcp::D, Pcp::U, Pcp::DR, Pcp::UR, Pcp::UL, Pcp::DL, Pcp::BR, Pcp::UB, Pcp::BL, Pcp::DB, Pcp::FR, Pcp::UF, Pcp::FL, Pcp::DF, Pcp::DBR, Pcp::UBR, Pcp::UBL, Pcp::DBL, Pcp::DFR, Pcp::UFR, Pcp::UFL, Pcp::DFL }, // BR
        { Pcp::B, Pcp::F, Pcp::D, Pcp::U, Pcp::L, Pcp::R, Pcp::DL, Pcp::DR, Pcp::UR, Pcp::UL, Pcp::DB, Pcp::BR, Pcp::UB, Pcp::BL, Pcp::DF, Pcp::FR, Pcp::UF, Pcp::FL, Pcp::DBL, Pcp::DBR, Pcp::UBR, Pcp::UBL, Pcp::DFL, Pcp::DFR, Pcp::UFR, Pcp::UFL }, // BD
        { Pcp::B, Pcp::F, Pcp::L, Pcp::R, Pcp::U, Pcp::D, Pcp::UL, Pcp::DL, Pcp::DR, Pcp::UR, Pcp::BL, Pcp::DB, Pcp::BR, Pcp::UB, Pcp::FL, Pcp::DF, Pcp::FR, Pcp::UF, Pcp::UBL, Pcp::DBL, Pcp::DBR, Pcp::UBR, Pcp::UFL, Pcp::DFL, Pcp::DFR, Pcp::UFR }, // BL
        { Pcp::R, Pcp::L, Pcp::U, Pcp::D, Pcp::F, Pcp::B, Pcp::UF, Pcp::UB, Pcp::DB, Pcp::DF, Pcp::UR, Pcp::BR, Pcp::DR, Pcp::FR, Pcp::UL, Pcp::BL, Pcp::DL, Pcp::FL, Pcp::UFR, Pcp::UBR, Pcp::DBR, Pcp::DFR, Pcp::UFL, Pcp::UBL, Pcp::DBL, Pcp::DFL }, // RU
        { Pcp::R, Pcp::L, Pcp::F, Pcp::B, Pcp::D, Pcp::U, Pcp::DF, Pcp::UF, Pcp::UB, Pcp::DB, Pcp::FR, Pcp::UR, Pcp::BR, Pcp::DR, Pcp::FL, Pcp::UL, Pcp::BL, Pcp::DL, Pcp::DFR, Pcp::UFR, Pcp::UBR, Pcp::DBR, Pcp::DFL, Pcp::UFL, Pcp::UBL, Pcp::DBL }, // RF
        { Pcp::R, Pcp::L, Pcp::D, Pcp::U, Pcp::B, Pcp::F, Pcp::DB, Pcp::DF, Pcp::UF, Pcp::UB, Pcp::DR, Pcp::FR, Pcp::UR, Pcp::BR, Pcp::DL, Pcp::FL, Pcp::UL, Pcp::BL, Pcp::DBR, Pcp::DFR, Pcp::UFR, Pcp::UBR, Pcp::DBL, Pcp::DFL, Pcp::UFL, Pcp::UBL }, // RD
        { Pcp::R, Pcp::L, Pcp::B, Pcp::F, Pcp::U, Pcp::D, Pcp::UB, Pcp::DB, Pcp::DF, Pcp::UF, Pcp::BR, Pcp::DR, Pcp::FR, Pcp::UR, Pcp::BL, Pcp::DL, Pcp::FL, Pcp::UL, Pcp::UBR, Pcp::DBR, Pcp::DFR, Pcp::UFR, Pcp::UBL, Pcp::DBL, Pcp::DFL, Pcp::UFL }, // RB
        { Pcp::L, Pcp::R, Pcp::U, Pcp::D, Pcp::B, Pcp::F, Pcp::UB, Pcp::UF, Pcp::DF, Pcp::DB, Pcp::UL, Pcp::FL, Pcp::DL, Pcp::BL, Pcp::UR, Pcp::FR, Pcp::DR, Pcp::BR, Pcp::UBL, Pcp::UFL, Pcp::DFL, Pcp::DBL, Pcp::UBR, Pcp::UFR, Pcp::DFR, Pcp::DBR }, // LU
        { Pcp::L, Pcp::R, Pcp::F, Pcp::B, Pcp::U, Pcp::D, Pcp::UF, Pcp::DF, Pcp::DB, Pcp::UB, Pcp::FL, Pcp::DL, Pcp::BL, Pcp::UL, Pcp::FR, Pcp::DR, Pcp::BR, Pcp::UR, Pcp::UFL, Pcp::DFL, Pcp::DBL, Pcp::UBL, Pcp::UFR, Pcp::DFR, Pcp::DBR, Pcp::UBR }, // LF
        { Pcp::L, Pcp::R, Pcp::D, Pcp::U, Pcp::F, Pcp::B, Pcp::DF, Pcp::DB, Pcp::UB, Pcp::UF, Pcp::DL, Pcp::BL, Pcp::UL, Pcp::FL, Pcp::DR, Pcp::BR, Pcp::UR, Pcp::FR, Pcp::DFL, Pcp::DBL, Pcp::UBL, Pcp::UFL, Pcp::DFR, Pcp::DBR, Pcp::UBR, Pcp::UFR }, // LD
        { Pcp::L, Pcp::R, Pcp::B, Pcp::F, Pcp::D, Pcp::U, Pcp::DB, Pcp::UB, Pcp::UF, Pcp::DF, Pcp::BL, Pcp::UL, Pcp::FL, Pcp::DL, Pcp::BR, Pcp::UR, Pcp::FR, Pcp::DR, Pcp::DBL, Pcp::UBL, Pcp::UFL, Pcp::DFL, Pcp::DBR, Pcp::UBR, Pcp::UFR, Pcp::DFR }  // LB
    };
	
	// Absolute piece positions from spin and piece positions
	const App Cube::app_Positions[24][26] =
	{
		{ App::U, App::D, App::F, App::B, App::R, App::L, App::FR, App::FL, App::BL, App::BR, App::UF, App::UL, App::UB, App::UR, App::DF, App::DL, App::DB, App::DR, App::UFR, App::UFL, App::UBL, App::UBR, App::DFR, App::DFL, App::DBL, App::DBR }, // UF
		{ App::U, App::D, App::L, App::R, App::F, App::B, App::FL, App::BL, App::BR, App::FR, App::UL, App::UB, App::UR, App::UF, App::DL, App::DB, App::DR, App::DF, App::UFL, App::UBL, App::UBR, App::UFR, App::DFL, App::DBL, App::DBR, App::DFR }, // UR
		{ App::U, App::D, App::B, App::F, App::L, App::R, App::BL, App::BR, App::FR, App::FL, App::UB, App::UR, App::UF, App::UL, App::DB, App::DR, App::DF, App::DL, App::UBL, App::UBR, App::UFR, App::UFL, App::DBL, App::DBR, App::DFR, App::DFL }, // UB
		{ App::U, App::D, App::R, App::L, App::B, App::F, App::BR, App::FR, App::FL, App::BL, App::UR, App::UF, App::UL, App::UB, App::DR, App::DF, App::DL, App::DB, App::UBR, App::UFR, App::UFL, App::UBL, App::DBR, App::DFR, App::DFL, App::DBL }, // UL
		{ App::D, App::U, App::F, App::B, App::L, App::R, App::FL, App::FR, App::BR, App::BL, App::DF, App::DR, App::DB, App::DL, App::UF, App::UR, App::UB, App::UL, App::DFL, App::DFR, App::DBR, App::DBL, App::UFL, App::UFR, App::UBR, App::UBL }, // DF
		{ App::D, App::U, App::R, App::L, App::F, App::B, App::FR, App::BR, App::BL, App::FL, App::DR, App::DB, App::DL, App::DF, App::UR, App::UB, App::UL, App::UF, App::DFR, App::DBR, App::DBL, App::DFL, App::UFR, App::UBR, App::UBL, App::UFL }, // DR
		{ App::D, App::U, App::B, App::F, App::R, App::L, App::BR, App::BL, App::FL, App::FR, App::DB, App::DL, App::DF, App::DR, App::UB, App::UL, App::UF, App::UR, App::DBR, App::DBL, App::DFL, App::DFR, App::UBR, App::UBL, App::UFL, App::UFR }, // DB
		{ App::D, App::U, App::L, App::R, App::B, App::F, App::BL, App::FL, App::FR, App::BR, App::DL, App::DF, App::DR, App::DB, App::UL, App::UF, App::UR, App::UB, App::DBL, App::DFL, App::DFR, App::DBR, App::UBL, App::UFL, App::UFR, App::UBR }, // DL
		{ App::F, App::B, App::U, App::D, App::L, App::R, App::UL, App::UR, App::DR, App::DL, App::UF, App::FR, App::DF, App::FL, App::UB, App::BR, App::DB, App::BL, App::UFL, App::UFR, App::DFR, App::DFL, App::UBL, App::UBR, App::DBR, App::DBL }, // FU
		{ App::R, App::L, App::U, App::D, App::F, App::B, App::UF, App::UB, App::DB, App::DF, App::UR, App::BR, App::DR, App::FR, App::UL, App::BL, App::DL, App::FL, App::UFR, App::UBR, App::DBR, App::DFR, App::UFL, App::UBL, App::DBL, App::DFL }, // FR
		{ App::B, App::F, App::U, App::D, App::R, App::L, App::UR, App::UL, App::DL, App::DR, App::UB, App::BL, App::DB, App::BR, App::UF, App::FL, App::DF, App::FR, App::UBR, App::UBL, App::DBL, App::DBR, App::UFR, App::UFL, App::DFL, App::DFR }, // FD
		{ App::L, App::R, App::U, App::D, App::B, App::F, App::UB, App::UF, App::DF, App::DB, App::UL, App::FL, App::DL, App::BL, App::UR, App::FR, App::DR, App::BR, App::UBL, App::UFL, App::DFL, App::DBL, App::UBR, App::UFR, App::DFR, App::DBR }, // FL
		{ App::F, App::B, App::D, App::U, App::R, App::L, App::DR, App::DL, App::UL, App::UR, App::DF, App::FL, App::UF, App::FR, App::DB, App::BL, App::UB, App::BR, App::DFR, App::DFL, App::UFL, App::UFR, App::DBR, App::DBL, App::UBL, App::UBR }, // BU
		{ App::L, App::R, App::D, App::U, App::F, App::B, App::DF, App::DB, App::UB, App::UF, App::DL, App::BL, App::UL, App::FL, App::DR, App::BR, App::UR, App::FR, App::DFL, App::DBL, App::UBL, App::UFL, App::DFR, App::DBR, App::UBR, App::UFR }, // BR
		{ App::B, App::F, App::D, App::U, App::L, App::R, App::DL, App::DR, App::UR, App::UL, App::DB, App::BR, App::UB, App::BL, App::DF, App::FR, App::UF, App::FL, App::DBL, App::DBR, App::UBR, App::UBL, App::DFL, App::DFR, App::UFR, App::UFL }, // BD
		{ App::R, App::L, App::D, App::U, App::B, App::F, App::DB, App::DF, App::UF, App::UB, App::DR, App::FR, App::UR, App::BR, App::DL, App::FL, App::UL, App::BL, App::DBR, App::DFR, App::UFR, App::UBR, App::DBL, App::DFL, App::UFL, App::UBL }, // BL
		{ App::F, App::B, App::R, App::L, App::U, App::D, App::UR, App::DR, App::DL, App::UL, App::FR, App::DF, App::FL, App::UF, App::BR, App::DB, App::BL, App::UB, App::UFR, App::DFR, App::DFL, App::UFL, App::UBR, App::DBR, App::DBL, App::UBL }, // RU
		{ App::L, App::R, App::F, App::B, App::U, App::D, App::UF, App::DF, App::DB, App::UB, App::FL, App::DL, App::BL, App::UL, App::FR, App::DR, App::BR, App::UR, App::UFL, App::DFL, App::DBL, App::UBL, App::UFR, App::DFR, App::DBR, App::UBR }, // RF
		{ App::B, App::F, App::L, App::R, App::U, App::D, App::UL, App::DL, App::DR, App::UR, App::BL, App::DB, App::BR, App::UB, App::FL, App::DF, App::FR, App::UF, App::UBL, App::DBL, App::DBR, App::UBR, App::UFL, App::DFL, App::DFR, App::UFR }, // RD
		{ App::R, App::L, App::B, App::F, App::U, App::D, App::UB, App::DB, App::DF, App::UF, App::BR, App::DR, App::FR, App::UR, App::BL, App::DL, App::FL, App::UL, App::UBR, App::DBR, App::DFR, App::UFR, App::UBL, App::DBL, App::DFL, App::UFL }, // RB
		{ App::F, App::B, App::L, App::R, App::D, App::U, App::DL, App::UL, App::UR, App::DR, App::FL, App::UF, App::FR, App::DF, App::BL, App::UB, App::BR, App::DB, App::DFL, App::UFL, App::UFR, App::DFR, App::DBL, App::UBL, App::UBR, App::DBR }, // LU
		{ App::R, App::L, App::F, App::B, App::D, App::U, App::DF, App::UF, App::UB, App::DB, App::FR, App::UR, App::BR, App::DR, App::FL, App::UL, App::BL, App::DL, App::DFR, App::UFR, App::UBR, App::DBR, App::DFL, App::UFL, App::UBL, App::DBL }, // LF
		{ App::B, App::F, App::R, App::L, App::D, App::U, App::DR, App::UR, App::UL, App::DL, App::BR, App::UB, App::BL, App::DB, App::FR, App::UF, App::FL, App::DF, App::DBR, App::UBR, App::UBL, App::DBL, App::DFR, App::UFR, App::UFL, App::DFL }, // LD
		{ App::L, App::R, App::B, App::F, App::D, App::U, App::DB, App::UB, App::UF, App::DF, App::BL, App::UL, App::FL, App::DL, App::BR, App::UR, App::FR, App::DR, App::DBL, App::UBL, App::UFL, App::DFL, App::DBR, App::UBR, App::UFR, App::DFR }  // LB
	};	

    // Sticker positions from spin and absolut sticker positions
    const Sps Cube::sps_Absolut[24][54] =
    {
        { Sps::U, Sps::D, Sps::F, Sps::B, Sps::R, Sps::L, Sps::FR_F, Sps::FR_R, Sps::FL_F, Sps::FL_L, Sps::BL_B, Sps::BL_L, Sps::BR_B, Sps::BR_R, Sps::UF_U, Sps::UF_F, Sps::UL_U, Sps::UL_L, Sps::UB_U, Sps::UB_B, Sps::UR_U, Sps::UR_R, Sps::DF_D, Sps::DF_F, Sps::DL_D, Sps::DL_L, Sps::DB_D, Sps::DB_B, Sps::DR_D, Sps::DR_R, Sps::UFR_U, Sps::UFR_F, Sps::UFR_R, Sps::UFL_U, Sps::UFL_F, Sps::UFL_L, Sps::UBL_U, Sps::UBL_B, Sps::UBL_L, Sps::UBR_U, Sps::UBR_B, Sps::UBR_R, Sps::DFR_D, Sps::DFR_F, Sps::DFR_R, Sps::DFL_D, Sps::DFL_F, Sps::DFL_L, Sps::DBL_D, Sps::DBL_B, Sps::DBL_L, Sps::DBR_D, Sps::DBR_B, Sps::DBR_R }, // UF
        { Sps::U, Sps::D, Sps::R, Sps::L, Sps::B, Sps::F, Sps::BR_R, Sps::BR_B, Sps::FR_R, Sps::FR_F, Sps::FL_L, Sps::FL_F, Sps::BL_L, Sps::BL_B, Sps::UR_U, Sps::UR_R, Sps::UF_U, Sps::UF_F, Sps::UL_U, Sps::UL_L, Sps::UB_U, Sps::UB_B, Sps::DR_D, Sps::DR_R, Sps::DF_D, Sps::DF_F, Sps::DL_D, Sps::DL_L, Sps::DB_D, Sps::DB_B, Sps::UBR_U, Sps::UBR_R, Sps::UBR_B, Sps::UFR_U, Sps::UFR_R, Sps::UFR_F, Sps::UFL_U, Sps::UFL_L, Sps::UFL_F, Sps::UBL_U, Sps::UBL_L, Sps::UBL_B, Sps::DBR_D, Sps::DBR_R, Sps::DBR_B, Sps::DFR_D, Sps::DFR_R, Sps::DFR_F, Sps::DFL_D, Sps::DFL_L, Sps::DFL_F, Sps::DBL_D, Sps::DBL_L, Sps::DBL_B }, // UR
        { Sps::U, Sps::D, Sps::B, Sps::F, Sps::L, Sps::R, Sps::BL_B, Sps::BL_L, Sps::BR_B, Sps::BR_R, Sps::FR_F, Sps::FR_R, Sps::FL_F, Sps::FL_L, Sps::UB_U, Sps::UB_B, Sps::UR_U, Sps::UR_R, Sps::UF_U, Sps::UF_F, Sps::UL_U, Sps::UL_L, Sps::DB_D, Sps::DB_B, Sps::DR_D, Sps::DR_R, Sps::DF_D, Sps::DF_F, Sps::DL_D, Sps::DL_L, Sps::UBL_U, Sps::UBL_B, Sps::UBL_L, Sps::UBR_U, Sps::UBR_B, Sps::UBR_R, Sps::UFR_U, Sps::UFR_F, Sps::UFR_R, Sps::UFL_U, Sps::UFL_F, Sps::UFL_L, Sps::DBL_D, Sps::DBL_B, Sps::DBL_L, Sps::DBR_D, Sps::DBR_B, Sps::DBR_R, Sps::DFR_D, Sps::DFR_F, Sps::DFR_R, Sps::DFL_D, Sps::DFL_F, Sps::DFL_L }, // UB
        { Sps::U, Sps::D, Sps::L, Sps::R, Sps::F, Sps::B, Sps::FL_L, Sps::FL_F, Sps::BL_L, Sps::BL_B, Sps::BR_R, Sps::BR_B, Sps::FR_R, Sps::FR_F, Sps::UL_U, Sps::UL_L, Sps::UB_U, Sps::UB_B, Sps::UR_U, Sps::UR_R, Sps::UF_U, Sps::UF_F, Sps::DL_D, Sps::DL_L, Sps::DB_D, Sps::DB_B, Sps::DR_D, Sps::DR_R, Sps::DF_D, Sps::DF_F, Sps::UFL_U, Sps::UFL_L, Sps::UFL_F, Sps::UBL_U, Sps::UBL_L, Sps::UBL_B, Sps::UBR_U, Sps::UBR_R, Sps::UBR_B, Sps::UFR_U, Sps::UFR_R, Sps::UFR_F, Sps::DFL_D, Sps::DFL_L, Sps::DFL_F, Sps::DBL_D, Sps::DBL_L, Sps::DBL_B, Sps::DBR_D, Sps::DBR_R, Sps::DBR_B, Sps::DFR_D, Sps::DFR_R, Sps::DFR_F }, // UL
        { Sps::D, Sps::U, Sps::F, Sps::B, Sps::L, Sps::R, Sps::FL_F, Sps::FL_L, Sps::FR_F, Sps::FR_R, Sps::BR_B, Sps::BR_R, Sps::BL_B, Sps::BL_L, Sps::DF_D, Sps::DF_F, Sps::DR_D, Sps::DR_R, Sps::DB_D, Sps::DB_B, Sps::DL_D, Sps::DL_L, Sps::UF_U, Sps::UF_F, Sps::UR_U, Sps::UR_R, Sps::UB_U, Sps::UB_B, Sps::UL_U, Sps::UL_L, Sps::DFL_D, Sps::DFL_F, Sps::DFL_L, Sps::DFR_D, Sps::DFR_F, Sps::DFR_R, Sps::DBR_D, Sps::DBR_B, Sps::DBR_R, Sps::DBL_D, Sps::DBL_B, Sps::DBL_L, Sps::UFL_U, Sps::UFL_F, Sps::UFL_L, Sps::UFR_U, Sps::UFR_F, Sps::UFR_R, Sps::UBR_U, Sps::UBR_B, Sps::UBR_R, Sps::UBL_U, Sps::UBL_B, Sps::UBL_L }, // DF
        { Sps::D, Sps::U, Sps::R, Sps::L, Sps::F, Sps::B, Sps::FR_R, Sps::FR_F, Sps::BR_R, Sps::BR_B, Sps::BL_L, Sps::BL_B, Sps::FL_L, Sps::FL_F, Sps::DR_D, Sps::DR_R, Sps::DB_D, Sps::DB_B, Sps::DL_D, Sps::DL_L, Sps::DF_D, Sps::DF_F, Sps::UR_U, Sps::UR_R, Sps::UB_U, Sps::UB_B, Sps::UL_U, Sps::UL_L, Sps::UF_U, Sps::UF_F, Sps::DFR_D, Sps::DFR_R, Sps::DFR_F, Sps::DBR_D, Sps::DBR_R, Sps::DBR_B, Sps::DBL_D, Sps::DBL_L, Sps::DBL_B, Sps::DFL_D, Sps::DFL_L, Sps::DFL_F, Sps::UFR_U, Sps::UFR_R, Sps::UFR_F, Sps::UBR_U, Sps::UBR_R, Sps::UBR_B, Sps::UBL_U, Sps::UBL_L, Sps::UBL_B, Sps::UFL_U, Sps::UFL_L, Sps::UFL_F }, // DR
        { Sps::D, Sps::U, Sps::B, Sps::F, Sps::R, Sps::L, Sps::BR_B, Sps::BR_R, Sps::BL_B, Sps::BL_L, Sps::FL_F, Sps::FL_L, Sps::FR_F, Sps::FR_R, Sps::DB_D, Sps::DB_B, Sps::DL_D, Sps::DL_L, Sps::DF_D, Sps::DF_F, Sps::DR_D, Sps::DR_R, Sps::UB_U, Sps::UB_B, Sps::UL_U, Sps::UL_L, Sps::UF_U, Sps::UF_F, Sps::UR_U, Sps::UR_R, Sps::DBR_D, Sps::DBR_B, Sps::DBR_R, Sps::DBL_D, Sps::DBL_B, Sps::DBL_L, Sps::DFL_D, Sps::DFL_F, Sps::DFL_L, Sps::DFR_D, Sps::DFR_F, Sps::DFR_R, Sps::UBR_U, Sps::UBR_B, Sps::UBR_R, Sps::UBL_U, Sps::UBL_B, Sps::UBL_L, Sps::UFL_U, Sps::UFL_F, Sps::UFL_L, Sps::UFR_U, Sps::UFR_F, Sps::UFR_R }, // DB
        { Sps::D, Sps::U, Sps::L, Sps::R, Sps::B, Sps::F, Sps::BL_L, Sps::BL_B, Sps::FL_L, Sps::FL_F, Sps::FR_R, Sps::FR_F, Sps::BR_R, Sps::BR_B, Sps::DL_D, Sps::DL_L, Sps::DF_D, Sps::DF_F, Sps::DR_D, Sps::DR_R, Sps::DB_D, Sps::DB_B, Sps::UL_U, Sps::UL_L, Sps::UF_U, Sps::UF_F, Sps::UR_U, Sps::UR_R, Sps::UB_U, Sps::UB_B, Sps::DBL_D, Sps::DBL_L, Sps::DBL_B, Sps::DFL_D, Sps::DFL_L, Sps::DFL_F, Sps::DFR_D, Sps::DFR_R, Sps::DFR_F, Sps::DBR_D, Sps::DBR_R, Sps::DBR_B, Sps::UBL_U, Sps::UBL_L, Sps::UBL_B, Sps::UFL_U, Sps::UFL_L, Sps::UFL_F, Sps::UFR_U, Sps::UFR_R, Sps::UFR_F, Sps::UBR_U, Sps::UBR_R, Sps::UBR_B }, // DL
        { Sps::F, Sps::B, Sps::U, Sps::D, Sps::L, Sps::R, Sps::UL_U, Sps::UL_L, Sps::UR_U, Sps::UR_R, Sps::DR_D, Sps::DR_R, Sps::DL_D, Sps::DL_L, Sps::UF_F, Sps::UF_U, Sps::FR_F, Sps::FR_R, Sps::DF_F, Sps::DF_D, Sps::FL_F, Sps::FL_L, Sps::UB_B, Sps::UB_U, Sps::BR_B, Sps::BR_R, Sps::DB_B, Sps::DB_D, Sps::BL_B, Sps::BL_L, Sps::UFL_F, Sps::UFL_U, Sps::UFL_L, Sps::UFR_F, Sps::UFR_U, Sps::UFR_R, Sps::DFR_F, Sps::DFR_D, Sps::DFR_R, Sps::DFL_F, Sps::DFL_D, Sps::DFL_L, Sps::UBL_B, Sps::UBL_U, Sps::UBL_L, Sps::UBR_B, Sps::UBR_U, Sps::UBR_R, Sps::DBR_B, Sps::DBR_D, Sps::DBR_R, Sps::DBL_B, Sps::DBL_D, Sps::DBL_L }, // FU
        { Sps::F, Sps::B, Sps::R, Sps::L, Sps::U, Sps::D, Sps::UR_R, Sps::UR_U, Sps::DR_R, Sps::DR_D, Sps::DL_L, Sps::DL_D, Sps::UL_L, Sps::UL_U, Sps::FR_F, Sps::FR_R, Sps::DF_F, Sps::DF_D, Sps::FL_F, Sps::FL_L, Sps::UF_F, Sps::UF_U, Sps::BR_B, Sps::BR_R, Sps::DB_B, Sps::DB_D, Sps::BL_B, Sps::BL_L, Sps::UB_B, Sps::UB_U, Sps::UFR_F, Sps::UFR_R, Sps::UFR_U, Sps::DFR_F, Sps::DFR_R, Sps::DFR_D, Sps::DFL_F, Sps::DFL_L, Sps::DFL_D, Sps::UFL_F, Sps::UFL_L, Sps::UFL_U, Sps::UBR_B, Sps::UBR_R, Sps::UBR_U, Sps::DBR_B, Sps::DBR_R, Sps::DBR_D, Sps::DBL_B, Sps::DBL_L, Sps::DBL_D, Sps::UBL_B, Sps::UBL_L, Sps::UBL_U }, // FR
        { Sps::F, Sps::B, Sps::D, Sps::U, Sps::R, Sps::L, Sps::DR_D, Sps::DR_R, Sps::DL_D, Sps::DL_L, Sps::UL_U, Sps::UL_L, Sps::UR_U, Sps::UR_R, Sps::DF_F, Sps::DF_D, Sps::FL_F, Sps::FL_L, Sps::UF_F, Sps::UF_U, Sps::FR_F, Sps::FR_R, Sps::DB_B, Sps::DB_D, Sps::BL_B, Sps::BL_L, Sps::UB_B, Sps::UB_U, Sps::BR_B, Sps::BR_R, Sps::DFR_F, Sps::DFR_D, Sps::DFR_R, Sps::DFL_F, Sps::DFL_D, Sps::DFL_L, Sps::UFL_F, Sps::UFL_U, Sps::UFL_L, Sps::UFR_F, Sps::UFR_U, Sps::UFR_R, Sps::DBR_B, Sps::DBR_D, Sps::DBR_R, Sps::DBL_B, Sps::DBL_D, Sps::DBL_L, Sps::UBL_B, Sps::UBL_U, Sps::UBL_L, Sps::UBR_B, Sps::UBR_U, Sps::UBR_R }, // FD
        { Sps::F, Sps::B, Sps::L, Sps::R, Sps::D, Sps::U, Sps::DL_L, Sps::DL_D, Sps::UL_L, Sps::UL_U, Sps::UR_R, Sps::UR_U, Sps::DR_R, Sps::DR_D, Sps::FL_F, Sps::FL_L, Sps::UF_F, Sps::UF_U, Sps::FR_F, Sps::FR_R, Sps::DF_F, Sps::DF_D, Sps::BL_B, Sps::BL_L, Sps::UB_B, Sps::UB_U, Sps::BR_B, Sps::BR_R, Sps::DB_B, Sps::DB_D, Sps::DFL_F, Sps::DFL_L, Sps::DFL_D, Sps::UFL_F, Sps::UFL_L, Sps::UFL_U, Sps::UFR_F, Sps::UFR_R, Sps::UFR_U, Sps::DFR_F, Sps::DFR_R, Sps::DFR_D, Sps::DBL_B, Sps::DBL_L, Sps::DBL_D, Sps::UBL_B, Sps::UBL_L, Sps::UBL_U, Sps::UBR_B, Sps::UBR_R, Sps::UBR_U, Sps::DBR_B, Sps::DBR_R, Sps::DBR_D }, // FL
        { Sps::B, Sps::F, Sps::U, Sps::D, Sps::R, Sps::L, Sps::UR_U, Sps::UR_R, Sps::UL_U, Sps::UL_L, Sps::DL_D, Sps::DL_L, Sps::DR_D, Sps::DR_R, Sps::UB_B, Sps::UB_U, Sps::BL_B, Sps::BL_L, Sps::DB_B, Sps::DB_D, Sps::BR_B, Sps::BR_R, Sps::UF_F, Sps::UF_U, Sps::FL_F, Sps::FL_L, Sps::DF_F, Sps::DF_D, Sps::FR_F, Sps::FR_R, Sps::UBR_B, Sps::UBR_U, Sps::UBR_R, Sps::UBL_B, Sps::UBL_U, Sps::UBL_L, Sps::DBL_B, Sps::DBL_D, Sps::DBL_L, Sps::DBR_B, Sps::DBR_D, Sps::DBR_R, Sps::UFR_F, Sps::UFR_U, Sps::UFR_R, Sps::UFL_F, Sps::UFL_U, Sps::UFL_L, Sps::DFL_F, Sps::DFL_D, Sps::DFL_L, Sps::DFR_F, Sps::DFR_D, Sps::DFR_R }, // BU
        { Sps::B, Sps::F, Sps::R, Sps::L, Sps::D, Sps::U, Sps::DR_R, Sps::DR_D, Sps::UR_R, Sps::UR_U, Sps::UL_L, Sps::UL_U, Sps::DL_L, Sps::DL_D, Sps::BR_B, Sps::BR_R, Sps::UB_B, Sps::UB_U, Sps::BL_B, Sps::BL_L, Sps::DB_B, Sps::DB_D, Sps::FR_F, Sps::FR_R, Sps::UF_F, Sps::UF_U, Sps::FL_F, Sps::FL_L, Sps::DF_F, Sps::DF_D, Sps::DBR_B, Sps::DBR_R, Sps::DBR_D, Sps::UBR_B, Sps::UBR_R, Sps::UBR_U, Sps::UBL_B, Sps::UBL_L, Sps::UBL_U, Sps::DBL_B, Sps::DBL_L, Sps::DBL_D, Sps::DFR_F, Sps::DFR_R, Sps::DFR_D, Sps::UFR_F, Sps::UFR_R, Sps::UFR_U, Sps::UFL_F, Sps::UFL_L, Sps::UFL_U, Sps::DFL_F, Sps::DFL_L, Sps::DFL_D }, // BR
        { Sps::B, Sps::F, Sps::D, Sps::U, Sps::L, Sps::R, Sps::DL_D, Sps::DL_L, Sps::DR_D, Sps::DR_R, Sps::UR_U, Sps::UR_R, Sps::UL_U, Sps::UL_L, Sps::DB_B, Sps::DB_D, Sps::BR_B, Sps::BR_R, Sps::UB_B, Sps::UB_U, Sps::BL_B, Sps::BL_L, Sps::DF_F, Sps::DF_D, Sps::FR_F, Sps::FR_R, Sps::UF_F, Sps::UF_U, Sps::FL_F, Sps::FL_L, Sps::DBL_B, Sps::DBL_D, Sps::DBL_L, Sps::DBR_B, Sps::DBR_D, Sps::DBR_R, Sps::UBR_B, Sps::UBR_U, Sps::UBR_R, Sps::UBL_B, Sps::UBL_U, Sps::UBL_L, Sps::DFL_F, Sps::DFL_D, Sps::DFL_L, Sps::DFR_F, Sps::DFR_D, Sps::DFR_R, Sps::UFR_F, Sps::UFR_U, Sps::UFR_R, Sps::UFL_F, Sps::UFL_U, Sps::UFL_L }, // BD
        { Sps::B, Sps::F, Sps::L, Sps::R, Sps::U, Sps::D, Sps::UL_L, Sps::UL_U, Sps::DL_L, Sps::DL_D, Sps::DR_R, Sps::DR_D, Sps::UR_R, Sps::UR_U, Sps::BL_B, Sps::BL_L, Sps::DB_B, Sps::DB_D, Sps::BR_B, Sps::BR_R, Sps::UB_B, Sps::UB_U, Sps::FL_F, Sps::FL_L, Sps::DF_F, Sps::DF_D, Sps::FR_F, Sps::FR_R, Sps::UF_F, Sps::UF_U, Sps::UBL_B, Sps::UBL_L, Sps::UBL_U, Sps::DBL_B, Sps::DBL_L, Sps::DBL_D, Sps::DBR_B, Sps::DBR_R, Sps::DBR_D, Sps::UBR_B, Sps::UBR_R, Sps::UBR_U, Sps::UFL_F, Sps::UFL_L, Sps::UFL_U, Sps::DFL_F, Sps::DFL_L, Sps::DFL_D, Sps::DFR_F, Sps::DFR_R, Sps::DFR_D, Sps::UFR_F, Sps::UFR_R, Sps::UFR_U }, // BL
        { Sps::R, Sps::L, Sps::U, Sps::D, Sps::F, Sps::B, Sps::UF_U, Sps::UF_F, Sps::UB_U, Sps::UB_B, Sps::DB_D, Sps::DB_B, Sps::DF_D, Sps::DF_F, Sps::UR_R, Sps::UR_U, Sps::BR_R, Sps::BR_B, Sps::DR_R, Sps::DR_D, Sps::FR_R, Sps::FR_F, Sps::UL_L, Sps::UL_U, Sps::BL_L, Sps::BL_B, Sps::DL_L, Sps::DL_D, Sps::FL_L, Sps::FL_F, Sps::UFR_R, Sps::UFR_U, Sps::UFR_F, Sps::UBR_R, Sps::UBR_U, Sps::UBR_B, Sps::DBR_R, Sps::DBR_D, Sps::DBR_B, Sps::DFR_R, Sps::DFR_D, Sps::DFR_F, Sps::UFL_L, Sps::UFL_U, Sps::UFL_F, Sps::UBL_L, Sps::UBL_U, Sps::UBL_B, Sps::DBL_L, Sps::DBL_D, Sps::DBL_B, Sps::DFL_L, Sps::DFL_D, Sps::DFL_F }, // RU
        { Sps::R, Sps::L, Sps::F, Sps::B, Sps::D, Sps::U, Sps::DF_F, Sps::DF_D, Sps::UF_F, Sps::UF_U, Sps::UB_B, Sps::UB_U, Sps::DB_B, Sps::DB_D, Sps::FR_R, Sps::FR_F, Sps::UR_R, Sps::UR_U, Sps::BR_R, Sps::BR_B, Sps::DR_R, Sps::DR_D, Sps::FL_L, Sps::FL_F, Sps::UL_L, Sps::UL_U, Sps::BL_L, Sps::BL_B, Sps::DL_L, Sps::DL_D, Sps::DFR_R, Sps::DFR_F, Sps::DFR_D, Sps::UFR_R, Sps::UFR_F, Sps::UFR_U, Sps::UBR_R, Sps::UBR_B, Sps::UBR_U, Sps::DBR_R, Sps::DBR_B, Sps::DBR_D, Sps::DFL_L, Sps::DFL_F, Sps::DFL_D, Sps::UFL_L, Sps::UFL_F, Sps::UFL_U, Sps::UBL_L, Sps::UBL_B, Sps::UBL_U, Sps::DBL_L, Sps::DBL_B, Sps::DBL_D }, // RF
        { Sps::R, Sps::L, Sps::D, Sps::U, Sps::B, Sps::F, Sps::DB_D, Sps::DB_B, Sps::DF_D, Sps::DF_F, Sps::UF_U, Sps::UF_F, Sps::UB_U, Sps::UB_B, Sps::DR_R, Sps::DR_D, Sps::FR_R, Sps::FR_F, Sps::UR_R, Sps::UR_U, Sps::BR_R, Sps::BR_B, Sps::DL_L, Sps::DL_D, Sps::FL_L, Sps::FL_F, Sps::UL_L, Sps::UL_U, Sps::BL_L, Sps::BL_B, Sps::DBR_R, Sps::DBR_D, Sps::DBR_B, Sps::DFR_R, Sps::DFR_D, Sps::DFR_F, Sps::UFR_R, Sps::UFR_U, Sps::UFR_F, Sps::UBR_R, Sps::UBR_U, Sps::UBR_B, Sps::DBL_L, Sps::DBL_D, Sps::DBL_B, Sps::DFL_L, Sps::DFL_D, Sps::DFL_F, Sps::UFL_L, Sps::UFL_U, Sps::UFL_F, Sps::UBL_L, Sps::UBL_U, Sps::UBL_B }, // RD
        { Sps::R, Sps::L, Sps::B, Sps::F, Sps::U, Sps::D, Sps::UB_B, Sps::UB_U, Sps::DB_B, Sps::DB_D, Sps::DF_F, Sps::DF_D, Sps::UF_F, Sps::UF_U, Sps::BR_R, Sps::BR_B, Sps::DR_R, Sps::DR_D, Sps::FR_R, Sps::FR_F, Sps::UR_R, Sps::UR_U, Sps::BL_L, Sps::BL_B, Sps::DL_L, Sps::DL_D, Sps::FL_L, Sps::FL_F, Sps::UL_L, Sps::UL_U, Sps::UBR_R, Sps::UBR_B, Sps::UBR_U, Sps::DBR_R, Sps::DBR_B, Sps::DBR_D, Sps::DFR_R, Sps::DFR_F, Sps::DFR_D, Sps::UFR_R, Sps::UFR_F, Sps::UFR_U, Sps::UBL_L, Sps::UBL_B, Sps::UBL_U, Sps::DBL_L, Sps::DBL_B, Sps::DBL_D, Sps::DFL_L, Sps::DFL_F, Sps::DFL_D, Sps::UFL_L, Sps::UFL_F, Sps::UFL_U }, // RB
        { Sps::L, Sps::R, Sps::U, Sps::D, Sps::B, Sps::F, Sps::UB_U, Sps::UB_B, Sps::UF_U, Sps::UF_F, Sps::DF_D, Sps::DF_F, Sps::DB_D, Sps::DB_B, Sps::UL_L, Sps::UL_U, Sps::FL_L, Sps::FL_F, Sps::DL_L, Sps::DL_D, Sps::BL_L, Sps::BL_B, Sps::UR_R, Sps::UR_U, Sps::FR_R, Sps::FR_F, Sps::DR_R, Sps::DR_D, Sps::BR_R, Sps::BR_B, Sps::UBL_L, Sps::UBL_U, Sps::UBL_B, Sps::UFL_L, Sps::UFL_U, Sps::UFL_F, Sps::DFL_L, Sps::DFL_D, Sps::DFL_F, Sps::DBL_L, Sps::DBL_D, Sps::DBL_B, Sps::UBR_R, Sps::UBR_U, Sps::UBR_B, Sps::UFR_R, Sps::UFR_U, Sps::UFR_F, Sps::DFR_R, Sps::DFR_D, Sps::DFR_F, Sps::DBR_R, Sps::DBR_D, Sps::DBR_B }, // LU
        { Sps::L, Sps::R, Sps::F, Sps::B, Sps::U, Sps::D, Sps::UF_F, Sps::UF_U, Sps::DF_F, Sps::DF_D, Sps::DB_B, Sps::DB_D, Sps::UB_B, Sps::UB_U, Sps::FL_L, Sps::FL_F, Sps::DL_L, Sps::DL_D, Sps::BL_L, Sps::BL_B, Sps::UL_L, Sps::UL_U, Sps::FR_R, Sps::FR_F, Sps::DR_R, Sps::DR_D, Sps::BR_R, Sps::BR_B, Sps::UR_R, Sps::UR_U, Sps::UFL_L, Sps::UFL_F, Sps::UFL_U, Sps::DFL_L, Sps::DFL_F, Sps::DFL_D, Sps::DBL_L, Sps::DBL_B, Sps::DBL_D, Sps::UBL_L, Sps::UBL_B, Sps::UBL_U, Sps::UFR_R, Sps::UFR_F, Sps::UFR_U, Sps::DFR_R, Sps::DFR_F, Sps::DFR_D, Sps::DBR_R, Sps::DBR_B, Sps::DBR_D, Sps::UBR_R, Sps::UBR_B, Sps::UBR_U }, // LF
        { Sps::L, Sps::R, Sps::D, Sps::U, Sps::F, Sps::B, Sps::DF_D, Sps::DF_F, Sps::DB_D, Sps::DB_B, Sps::UB_U, Sps::UB_B, Sps::UF_U, Sps::UF_F, Sps::DL_L, Sps::DL_D, Sps::BL_L, Sps::BL_B, Sps::UL_L, Sps::UL_U, Sps::FL_L, Sps::FL_F, Sps::DR_R, Sps::DR_D, Sps::BR_R, Sps::BR_B, Sps::UR_R, Sps::UR_U, Sps::FR_R, Sps::FR_F, Sps::DFL_L, Sps::DFL_D, Sps::DFL_F, Sps::DBL_L, Sps::DBL_D, Sps::DBL_B, Sps::UBL_L, Sps::UBL_U, Sps::UBL_B, Sps::UFL_L, Sps::UFL_U, Sps::UFL_F, Sps::DFR_R, Sps::DFR_D, Sps::DFR_F, Sps::DBR_R, Sps::DBR_D, Sps::DBR_B, Sps::UBR_R, Sps::UBR_U, Sps::UBR_B, Sps::UFR_R, Sps::UFR_U, Sps::UFR_F }, // LD
        { Sps::L, Sps::R, Sps::B, Sps::F, Sps::D, Sps::U, Sps::DB_B, Sps::DB_D, Sps::UB_B, Sps::UB_U, Sps::UF_F, Sps::UF_U, Sps::DF_F, Sps::DF_D, Sps::BL_L, Sps::BL_B, Sps::UL_L, Sps::UL_U, Sps::FL_L, Sps::FL_F, Sps::DL_L, Sps::DL_D, Sps::BR_R, Sps::BR_B, Sps::UR_R, Sps::UR_U, Sps::FR_R, Sps::FR_F, Sps::DR_R, Sps::DR_D, Sps::DBL_L, Sps::DBL_B, Sps::DBL_D, Sps::UBL_L, Sps::UBL_B, Sps::UBL_U, Sps::UFL_L, Sps::UFL_F, Sps::UFL_U, Sps::DFL_L, Sps::DFL_F, Sps::DFL_D, Sps::DBR_R, Sps::DBR_B, Sps::DBR_D, Sps::UBR_R, Sps::UBR_B, Sps::UBR_U, Sps::UFR_R, Sps::UFR_F, Sps::UFR_U, Sps::DFR_R, Sps::DFR_F, Sps::DFR_D }  // LB
    };
	
	// Absolut sticker positions from spin and sticker positions
	const Asp Cube::asp_Positions[24][54] =
	{
		{ Asp::U, Asp::D, Asp::F, Asp::B, Asp::R, Asp::L, Asp::FR_F, Asp::FR_R, Asp::FL_F, Asp::FL_L, Asp::BL_B, Asp::BL_L, Asp::BR_B, Asp::BR_R, Asp::UF_U, Asp::UF_F, Asp::UL_U, Asp::UL_L, Asp::UB_U, Asp::UB_B, Asp::UR_U, Asp::UR_R, Asp::DF_D, Asp::DF_F, Asp::DL_D, Asp::DL_L, Asp::DB_D, Asp::DB_B, Asp::DR_D, Asp::DR_R, Asp::UFR_U, Asp::UFR_F, Asp::UFR_R, Asp::UFL_U, Asp::UFL_F, Asp::UFL_L, Asp::UBL_U, Asp::UBL_B, Asp::UBL_L, Asp::UBR_U, Asp::UBR_B, Asp::UBR_R, Asp::DFR_D, Asp::DFR_F, Asp::DFR_R, Asp::DFL_D, Asp::DFL_F, Asp::DFL_L, Asp::DBL_D, Asp::DBL_B, Asp::DBL_L, Asp::DBR_D, Asp::DBR_B, Asp::DBR_R },
		{ Asp::U, Asp::D, Asp::L, Asp::R, Asp::F, Asp::B, Asp::FL_L, Asp::FL_F, Asp::BL_L, Asp::BL_B, Asp::BR_R, Asp::BR_B, Asp::FR_R, Asp::FR_F, Asp::UL_U, Asp::UL_L, Asp::UB_U, Asp::UB_B, Asp::UR_U, Asp::UR_R, Asp::UF_U, Asp::UF_F, Asp::DL_D, Asp::DL_L, Asp::DB_D, Asp::DB_B, Asp::DR_D, Asp::DR_R, Asp::DF_D, Asp::DF_F, Asp::UFL_U, Asp::UFL_L, Asp::UFL_F, Asp::UBL_U, Asp::UBL_L, Asp::UBL_B, Asp::UBR_U, Asp::UBR_R, Asp::UBR_B, Asp::UFR_U, Asp::UFR_R, Asp::UFR_F, Asp::DFL_D, Asp::DFL_L, Asp::DFL_F, Asp::DBL_D, Asp::DBL_L, Asp::DBL_B, Asp::DBR_D, Asp::DBR_R, Asp::DBR_B, Asp::DFR_D, Asp::DFR_R, Asp::DFR_F },
		{ Asp::U, Asp::D, Asp::B, Asp::F, Asp::L, Asp::R, Asp::BL_B, Asp::BL_L, Asp::BR_B, Asp::BR_R, Asp::FR_F, Asp::FR_R, Asp::FL_F, Asp::FL_L, Asp::UB_U, Asp::UB_B, Asp::UR_U, Asp::UR_R, Asp::UF_U, Asp::UF_F, Asp::UL_U, Asp::UL_L, Asp::DB_D, Asp::DB_B, Asp::DR_D, Asp::DR_R, Asp::DF_D, Asp::DF_F, Asp::DL_D, Asp::DL_L, Asp::UBL_U, Asp::UBL_B, Asp::UBL_L, Asp::UBR_U, Asp::UBR_B, Asp::UBR_R, Asp::UFR_U, Asp::UFR_F, Asp::UFR_R, Asp::UFL_U, Asp::UFL_F, Asp::UFL_L, Asp::DBL_D, Asp::DBL_B, Asp::DBL_L, Asp::DBR_D, Asp::DBR_B, Asp::DBR_R, Asp::DFR_D, Asp::DFR_F, Asp::DFR_R, Asp::DFL_D, Asp::DFL_F, Asp::DFL_L },
		{ Asp::U, Asp::D, Asp::R, Asp::L, Asp::B, Asp::F, Asp::BR_R, Asp::BR_B, Asp::FR_R, Asp::FR_F, Asp::FL_L, Asp::FL_F, Asp::BL_L, Asp::BL_B, Asp::UR_U, Asp::UR_R, Asp::UF_U, Asp::UF_F, Asp::UL_U, Asp::UL_L, Asp::UB_U, Asp::UB_B, Asp::DR_D, Asp::DR_R, Asp::DF_D, Asp::DF_F, Asp::DL_D, Asp::DL_L, Asp::DB_D, Asp::DB_B, Asp::UBR_U, Asp::UBR_R, Asp::UBR_B, Asp::UFR_U, Asp::UFR_R, Asp::UFR_F, Asp::UFL_U, Asp::UFL_L, Asp::UFL_F, Asp::UBL_U, Asp::UBL_L, Asp::UBL_B, Asp::DBR_D, Asp::DBR_R, Asp::DBR_B, Asp::DFR_D, Asp::DFR_R, Asp::DFR_F, Asp::DFL_D, Asp::DFL_L, Asp::DFL_F, Asp::DBL_D, Asp::DBL_L, Asp::DBL_B },
		{ Asp::D, Asp::U, Asp::F, Asp::B, Asp::L, Asp::R, Asp::FL_F, Asp::FL_L, Asp::FR_F, Asp::FR_R, Asp::BR_B, Asp::BR_R, Asp::BL_B, Asp::BL_L, Asp::DF_D, Asp::DF_F, Asp::DR_D, Asp::DR_R, Asp::DB_D, Asp::DB_B, Asp::DL_D, Asp::DL_L, Asp::UF_U, Asp::UF_F, Asp::UR_U, Asp::UR_R, Asp::UB_U, Asp::UB_B, Asp::UL_U, Asp::UL_L, Asp::DFL_D, Asp::DFL_F, Asp::DFL_L, Asp::DFR_D, Asp::DFR_F, Asp::DFR_R, Asp::DBR_D, Asp::DBR_B, Asp::DBR_R, Asp::DBL_D, Asp::DBL_B, Asp::DBL_L, Asp::UFL_U, Asp::UFL_F, Asp::UFL_L, Asp::UFR_U, Asp::UFR_F, Asp::UFR_R, Asp::UBR_U, Asp::UBR_B, Asp::UBR_R, Asp::UBL_U, Asp::UBL_B, Asp::UBL_L },
		{ Asp::D, Asp::U, Asp::R, Asp::L, Asp::F, Asp::B, Asp::FR_R, Asp::FR_F, Asp::BR_R, Asp::BR_B, Asp::BL_L, Asp::BL_B, Asp::FL_L, Asp::FL_F, Asp::DR_D, Asp::DR_R, Asp::DB_D, Asp::DB_B, Asp::DL_D, Asp::DL_L, Asp::DF_D, Asp::DF_F, Asp::UR_U, Asp::UR_R, Asp::UB_U, Asp::UB_B, Asp::UL_U, Asp::UL_L, Asp::UF_U, Asp::UF_F, Asp::DFR_D, Asp::DFR_R, Asp::DFR_F, Asp::DBR_D, Asp::DBR_R, Asp::DBR_B, Asp::DBL_D, Asp::DBL_L, Asp::DBL_B, Asp::DFL_D, Asp::DFL_L, Asp::DFL_F, Asp::UFR_U, Asp::UFR_R, Asp::UFR_F, Asp::UBR_U, Asp::UBR_R, Asp::UBR_B, Asp::UBL_U, Asp::UBL_L, Asp::UBL_B, Asp::UFL_U, Asp::UFL_L, Asp::UFL_F },
		{ Asp::D, Asp::U, Asp::B, Asp::F, Asp::R, Asp::L, Asp::BR_B, Asp::BR_R, Asp::BL_B, Asp::BL_L, Asp::FL_F, Asp::FL_L, Asp::FR_F, Asp::FR_R, Asp::DB_D, Asp::DB_B, Asp::DL_D, Asp::DL_L, Asp::DF_D, Asp::DF_F, Asp::DR_D, Asp::DR_R, Asp::UB_U, Asp::UB_B, Asp::UL_U, Asp::UL_L, Asp::UF_U, Asp::UF_F, Asp::UR_U, Asp::UR_R, Asp::DBR_D, Asp::DBR_B, Asp::DBR_R, Asp::DBL_D, Asp::DBL_B, Asp::DBL_L, Asp::DFL_D, Asp::DFL_F, Asp::DFL_L, Asp::DFR_D, Asp::DFR_F, Asp::DFR_R, Asp::UBR_U, Asp::UBR_B, Asp::UBR_R, Asp::UBL_U, Asp::UBL_B, Asp::UBL_L, Asp::UFL_U, Asp::UFL_F, Asp::UFL_L, Asp::UFR_U, Asp::UFR_F, Asp::UFR_R },
		{ Asp::D, Asp::U, Asp::L, Asp::R, Asp::B, Asp::F, Asp::BL_L, Asp::BL_B, Asp::FL_L, Asp::FL_F, Asp::FR_R, Asp::FR_F, Asp::BR_R, Asp::BR_B, Asp::DL_D, Asp::DL_L, Asp::DF_D, Asp::DF_F, Asp::DR_D, Asp::DR_R, Asp::DB_D, Asp::DB_B, Asp::UL_U, Asp::UL_L, Asp::UF_U, Asp::UF_F, Asp::UR_U, Asp::UR_R, Asp::UB_U, Asp::UB_B, Asp::DBL_D, Asp::DBL_L, Asp::DBL_B, Asp::DFL_D, Asp::DFL_L, Asp::DFL_F, Asp::DFR_D, Asp::DFR_R, Asp::DFR_F, Asp::DBR_D, Asp::DBR_R, Asp::DBR_B, Asp::UBL_U, Asp::UBL_L, Asp::UBL_B, Asp::UFL_U, Asp::UFL_L, Asp::UFL_F, Asp::UFR_U, Asp::UFR_R, Asp::UFR_F, Asp::UBR_U, Asp::UBR_R, Asp::UBR_B },
		{ Asp::F, Asp::B, Asp::U, Asp::D, Asp::L, Asp::R, Asp::UL_U, Asp::UL_L, Asp::UR_U, Asp::UR_R, Asp::DR_D, Asp::DR_R, Asp::DL_D, Asp::DL_L, Asp::UF_F, Asp::UF_U, Asp::FR_F, Asp::FR_R, Asp::DF_F, Asp::DF_D, Asp::FL_F, Asp::FL_L, Asp::UB_B, Asp::UB_U, Asp::BR_B, Asp::BR_R, Asp::DB_B, Asp::DB_D, Asp::BL_B, Asp::BL_L, Asp::UFL_F, Asp::UFL_U, Asp::UFL_L, Asp::UFR_F, Asp::UFR_U, Asp::UFR_R, Asp::DFR_F, Asp::DFR_D, Asp::DFR_R, Asp::DFL_F, Asp::DFL_D, Asp::DFL_L, Asp::UBL_B, Asp::UBL_U, Asp::UBL_L, Asp::UBR_B, Asp::UBR_U, Asp::UBR_R, Asp::DBR_B, Asp::DBR_D, Asp::DBR_R, Asp::DBL_B, Asp::DBL_D, Asp::DBL_L },
		{ Asp::R, Asp::L, Asp::U, Asp::D, Asp::F, Asp::B, Asp::UF_U, Asp::UF_F, Asp::UB_U, Asp::UB_B, Asp::DB_D, Asp::DB_B, Asp::DF_D, Asp::DF_F, Asp::UR_R, Asp::UR_U, Asp::BR_R, Asp::BR_B, Asp::DR_R, Asp::DR_D, Asp::FR_R, Asp::FR_F, Asp::UL_L, Asp::UL_U, Asp::BL_L, Asp::BL_B, Asp::DL_L, Asp::DL_D, Asp::FL_L, Asp::FL_F, Asp::UFR_R, Asp::UFR_U, Asp::UFR_F, Asp::UBR_R, Asp::UBR_U, Asp::UBR_B, Asp::DBR_R, Asp::DBR_D, Asp::DBR_B, Asp::DFR_R, Asp::DFR_D, Asp::DFR_F, Asp::UFL_L, Asp::UFL_U, Asp::UFL_F, Asp::UBL_L, Asp::UBL_U, Asp::UBL_B, Asp::DBL_L, Asp::DBL_D, Asp::DBL_B, Asp::DFL_L, Asp::DFL_D, Asp::DFL_F },
		{ Asp::B, Asp::F, Asp::U, Asp::D, Asp::R, Asp::L, Asp::UR_U, Asp::UR_R, Asp::UL_U, Asp::UL_L, Asp::DL_D, Asp::DL_L, Asp::DR_D, Asp::DR_R, Asp::UB_B, Asp::UB_U, Asp::BL_B, Asp::BL_L, Asp::DB_B, Asp::DB_D, Asp::BR_B, Asp::BR_R, Asp::UF_F, Asp::UF_U, Asp::FL_F, Asp::FL_L, Asp::DF_F, Asp::DF_D, Asp::FR_F, Asp::FR_R, Asp::UBR_B, Asp::UBR_U, Asp::UBR_R, Asp::UBL_B, Asp::UBL_U, Asp::UBL_L, Asp::DBL_B, Asp::DBL_D, Asp::DBL_L, Asp::DBR_B, Asp::DBR_D, Asp::DBR_R, Asp::UFR_F, Asp::UFR_U, Asp::UFR_R, Asp::UFL_F, Asp::UFL_U, Asp::UFL_L, Asp::DFL_F, Asp::DFL_D, Asp::DFL_L, Asp::DFR_F, Asp::DFR_D, Asp::DFR_R },
		{ Asp::L, Asp::R, Asp::U, Asp::D, Asp::B, Asp::F, Asp::UB_U, Asp::UB_B, Asp::UF_U, Asp::UF_F, Asp::DF_D, Asp::DF_F, Asp::DB_D, Asp::DB_B, Asp::UL_L, Asp::UL_U, Asp::FL_L, Asp::FL_F, Asp::DL_L, Asp::DL_D, Asp::BL_L, Asp::BL_B, Asp::UR_R, Asp::UR_U, Asp::FR_R, Asp::FR_F, Asp::DR_R, Asp::DR_D, Asp::BR_R, Asp::BR_B, Asp::UBL_L, Asp::UBL_U, Asp::UBL_B, Asp::UFL_L, Asp::UFL_U, Asp::UFL_F, Asp::DFL_L, Asp::DFL_D, Asp::DFL_F, Asp::DBL_L, Asp::DBL_D, Asp::DBL_B, Asp::UBR_R, Asp::UBR_U, Asp::UBR_B, Asp::UFR_R, Asp::UFR_U, Asp::UFR_F, Asp::DFR_R, Asp::DFR_D, Asp::DFR_F, Asp::DBR_R, Asp::DBR_D, Asp::DBR_B },
		{ Asp::F, Asp::B, Asp::D, Asp::U, Asp::R, Asp::L, Asp::DR_D, Asp::DR_R, Asp::DL_D, Asp::DL_L, Asp::UL_U, Asp::UL_L, Asp::UR_U, Asp::UR_R, Asp::DF_F, Asp::DF_D, Asp::FL_F, Asp::FL_L, Asp::UF_F, Asp::UF_U, Asp::FR_F, Asp::FR_R, Asp::DB_B, Asp::DB_D, Asp::BL_B, Asp::BL_L, Asp::UB_B, Asp::UB_U, Asp::BR_B, Asp::BR_R, Asp::DFR_F, Asp::DFR_D, Asp::DFR_R, Asp::DFL_F, Asp::DFL_D, Asp::DFL_L, Asp::UFL_F, Asp::UFL_U, Asp::UFL_L, Asp::UFR_F, Asp::UFR_U, Asp::UFR_R, Asp::DBR_B, Asp::DBR_D, Asp::DBR_R, Asp::DBL_B, Asp::DBL_D, Asp::DBL_L, Asp::UBL_B, Asp::UBL_U, Asp::UBL_L, Asp::UBR_B, Asp::UBR_U, Asp::UBR_R },
		{ Asp::L, Asp::R, Asp::D, Asp::U, Asp::F, Asp::B, Asp::DF_D, Asp::DF_F, Asp::DB_D, Asp::DB_B, Asp::UB_U, Asp::UB_B, Asp::UF_U, Asp::UF_F, Asp::DL_L, Asp::DL_D, Asp::BL_L, Asp::BL_B, Asp::UL_L, Asp::UL_U, Asp::FL_L, Asp::FL_F, Asp::DR_R, Asp::DR_D, Asp::BR_R, Asp::BR_B, Asp::UR_R, Asp::UR_U, Asp::FR_R, Asp::FR_F, Asp::DFL_L, Asp::DFL_D, Asp::DFL_F, Asp::DBL_L, Asp::DBL_D, Asp::DBL_B, Asp::UBL_L, Asp::UBL_U, Asp::UBL_B, Asp::UFL_L, Asp::UFL_U, Asp::UFL_F, Asp::DFR_R, Asp::DFR_D, Asp::DFR_F, Asp::DBR_R, Asp::DBR_D, Asp::DBR_B, Asp::UBR_R, Asp::UBR_U, Asp::UBR_B, Asp::UFR_R, Asp::UFR_U, Asp::UFR_F },
		{ Asp::B, Asp::F, Asp::D, Asp::U, Asp::L, Asp::R, Asp::DL_D, Asp::DL_L, Asp::DR_D, Asp::DR_R, Asp::UR_U, Asp::UR_R, Asp::UL_U, Asp::UL_L, Asp::DB_B, Asp::DB_D, Asp::BR_B, Asp::BR_R, Asp::UB_B, Asp::UB_U, Asp::BL_B, Asp::BL_L, Asp::DF_F, Asp::DF_D, Asp::FR_F, Asp::FR_R, Asp::UF_F, Asp::UF_U, Asp::FL_F, Asp::FL_L, Asp::DBL_B, Asp::DBL_D, Asp::DBL_L, Asp::DBR_B, Asp::DBR_D, Asp::DBR_R, Asp::UBR_B, Asp::UBR_U, Asp::UBR_R, Asp::UBL_B, Asp::UBL_U, Asp::UBL_L, Asp::DFL_F, Asp::DFL_D, Asp::DFL_L, Asp::DFR_F, Asp::DFR_D, Asp::DFR_R, Asp::UFR_F, Asp::UFR_U, Asp::UFR_R, Asp::UFL_F, Asp::UFL_U, Asp::UFL_L },
		{ Asp::R, Asp::L, Asp::D, Asp::U, Asp::B, Asp::F, Asp::DB_D, Asp::DB_B, Asp::DF_D, Asp::DF_F, Asp::UF_U, Asp::UF_F, Asp::UB_U, Asp::UB_B, Asp::DR_R, Asp::DR_D, Asp::FR_R, Asp::FR_F, Asp::UR_R, Asp::UR_U, Asp::BR_R, Asp::BR_B, Asp::DL_L, Asp::DL_D, Asp::FL_L, Asp::FL_F, Asp::UL_L, Asp::UL_U, Asp::BL_L, Asp::BL_B, Asp::DBR_R, Asp::DBR_D, Asp::DBR_B, Asp::DFR_R, Asp::DFR_D, Asp::DFR_F, Asp::UFR_R, Asp::UFR_U, Asp::UFR_F, Asp::UBR_R, Asp::UBR_U, Asp::UBR_B, Asp::DBL_L, Asp::DBL_D, Asp::DBL_B, Asp::DFL_L, Asp::DFL_D, Asp::DFL_F, Asp::UFL_L, Asp::UFL_U, Asp::UFL_F, Asp::UBL_L, Asp::UBL_U, Asp::UBL_B },
		{ Asp::F, Asp::B, Asp::R, Asp::L, Asp::U, Asp::D, Asp::UR_R, Asp::UR_U, Asp::DR_R, Asp::DR_D, Asp::DL_L, Asp::DL_D, Asp::UL_L, Asp::UL_U, Asp::FR_F, Asp::FR_R, Asp::DF_F, Asp::DF_D, Asp::FL_F, Asp::FL_L, Asp::UF_F, Asp::UF_U, Asp::BR_B, Asp::BR_R, Asp::DB_B, Asp::DB_D, Asp::BL_B, Asp::BL_L, Asp::UB_B, Asp::UB_U, Asp::UFR_F, Asp::UFR_R, Asp::UFR_U, Asp::DFR_F, Asp::DFR_R, Asp::DFR_D, Asp::DFL_F, Asp::DFL_L, Asp::DFL_D, Asp::UFL_F, Asp::UFL_L, Asp::UFL_U, Asp::UBR_B, Asp::UBR_R, Asp::UBR_U, Asp::DBR_B, Asp::DBR_R, Asp::DBR_D, Asp::DBL_B, Asp::DBL_L, Asp::DBL_D, Asp::UBL_B, Asp::UBL_L, Asp::UBL_U },
		{ Asp::L, Asp::R, Asp::F, Asp::B, Asp::U, Asp::D, Asp::UF_F, Asp::UF_U, Asp::DF_F, Asp::DF_D, Asp::DB_B, Asp::DB_D, Asp::UB_B, Asp::UB_U, Asp::FL_L, Asp::FL_F, Asp::DL_L, Asp::DL_D, Asp::BL_L, Asp::BL_B, Asp::UL_L, Asp::UL_U, Asp::FR_R, Asp::FR_F, Asp::DR_R, Asp::DR_D, Asp::BR_R, Asp::BR_B, Asp::UR_R, Asp::UR_U, Asp::UFL_L, Asp::UFL_F, Asp::UFL_U, Asp::DFL_L, Asp::DFL_F, Asp::DFL_D, Asp::DBL_L, Asp::DBL_B, Asp::DBL_D, Asp::UBL_L, Asp::UBL_B, Asp::UBL_U, Asp::UFR_R, Asp::UFR_F, Asp::UFR_U, Asp::DFR_R, Asp::DFR_F, Asp::DFR_D, Asp::DBR_R, Asp::DBR_B, Asp::DBR_D, Asp::UBR_R, Asp::UBR_B, Asp::UBR_U },
		{ Asp::B, Asp::F, Asp::L, Asp::R, Asp::U, Asp::D, Asp::UL_L, Asp::UL_U, Asp::DL_L, Asp::DL_D, Asp::DR_R, Asp::DR_D, Asp::UR_R, Asp::UR_U, Asp::BL_B, Asp::BL_L, Asp::DB_B, Asp::DB_D, Asp::BR_B, Asp::BR_R, Asp::UB_B, Asp::UB_U, Asp::FL_F, Asp::FL_L, Asp::DF_F, Asp::DF_D, Asp::FR_F, Asp::FR_R, Asp::UF_F, Asp::UF_U, Asp::UBL_B, Asp::UBL_L, Asp::UBL_U, Asp::DBL_B, Asp::DBL_L, Asp::DBL_D, Asp::DBR_B, Asp::DBR_R, Asp::DBR_D, Asp::UBR_B, Asp::UBR_R, Asp::UBR_U, Asp::UFL_F, Asp::UFL_L, Asp::UFL_U, Asp::DFL_F, Asp::DFL_L, Asp::DFL_D, Asp::DFR_F, Asp::DFR_R, Asp::DFR_D, Asp::UFR_F, Asp::UFR_R, Asp::UFR_U },
		{ Asp::R, Asp::L, Asp::B, Asp::F, Asp::U, Asp::D, Asp::UB_B, Asp::UB_U, Asp::DB_B, Asp::DB_D, Asp::DF_F, Asp::DF_D, Asp::UF_F, Asp::UF_U, Asp::BR_R, Asp::BR_B, Asp::DR_R, Asp::DR_D, Asp::FR_R, Asp::FR_F, Asp::UR_R, Asp::UR_U, Asp::BL_L, Asp::BL_B, Asp::DL_L, Asp::DL_D, Asp::FL_L, Asp::FL_F, Asp::UL_L, Asp::UL_U, Asp::UBR_R, Asp::UBR_B, Asp::UBR_U, Asp::DBR_R, Asp::DBR_B, Asp::DBR_D, Asp::DFR_R, Asp::DFR_F, Asp::DFR_D, Asp::UFR_R, Asp::UFR_F, Asp::UFR_U, Asp::UBL_L, Asp::UBL_B, Asp::UBL_U, Asp::DBL_L, Asp::DBL_B, Asp::DBL_D, Asp::DFL_L, Asp::DFL_F, Asp::DFL_D, Asp::UFL_L, Asp::UFL_F, Asp::UFL_U },
		{ Asp::F, Asp::B, Asp::L, Asp::R, Asp::D, Asp::U, Asp::DL_L, Asp::DL_D, Asp::UL_L, Asp::UL_U, Asp::UR_R, Asp::UR_U, Asp::DR_R, Asp::DR_D, Asp::FL_F, Asp::FL_L, Asp::UF_F, Asp::UF_U, Asp::FR_F, Asp::FR_R, Asp::DF_F, Asp::DF_D, Asp::BL_B, Asp::BL_L, Asp::UB_B, Asp::UB_U, Asp::BR_B, Asp::BR_R, Asp::DB_B, Asp::DB_D, Asp::DFL_F, Asp::DFL_L, Asp::DFL_D, Asp::UFL_F, Asp::UFL_L, Asp::UFL_U, Asp::UFR_F, Asp::UFR_R, Asp::UFR_U, Asp::DFR_F, Asp::DFR_R, Asp::DFR_D, Asp::DBL_B, Asp::DBL_L, Asp::DBL_D, Asp::UBL_B, Asp::UBL_L, Asp::UBL_U, Asp::UBR_B, Asp::UBR_R, Asp::UBR_U, Asp::DBR_B, Asp::DBR_R, Asp::DBR_D },
		{ Asp::R, Asp::L, Asp::F, Asp::B, Asp::D, Asp::U, Asp::DF_F, Asp::DF_D, Asp::UF_F, Asp::UF_U, Asp::UB_B, Asp::UB_U, Asp::DB_B, Asp::DB_D, Asp::FR_R, Asp::FR_F, Asp::UR_R, Asp::UR_U, Asp::BR_R, Asp::BR_B, Asp::DR_R, Asp::DR_D, Asp::FL_L, Asp::FL_F, Asp::UL_L, Asp::UL_U, Asp::BL_L, Asp::BL_B, Asp::DL_L, Asp::DL_D, Asp::DFR_R, Asp::DFR_F, Asp::DFR_D, Asp::UFR_R, Asp::UFR_F, Asp::UFR_U, Asp::UBR_R, Asp::UBR_B, Asp::UBR_U, Asp::DBR_R, Asp::DBR_B, Asp::DBR_D, Asp::DFL_L, Asp::DFL_F, Asp::DFL_D, Asp::UFL_L, Asp::UFL_F, Asp::UFL_U, Asp::UBL_L, Asp::UBL_B, Asp::UBL_U, Asp::DBL_L, Asp::DBL_B, Asp::DBL_D },
		{ Asp::B, Asp::F, Asp::R, Asp::L, Asp::D, Asp::U, Asp::DR_R, Asp::DR_D, Asp::UR_R, Asp::UR_U, Asp::UL_L, Asp::UL_U, Asp::DL_L, Asp::DL_D, Asp::BR_B, Asp::BR_R, Asp::UB_B, Asp::UB_U, Asp::BL_B, Asp::BL_L, Asp::DB_B, Asp::DB_D, Asp::FR_F, Asp::FR_R, Asp::UF_F, Asp::UF_U, Asp::FL_F, Asp::FL_L, Asp::DF_F, Asp::DF_D, Asp::DBR_B, Asp::DBR_R, Asp::DBR_D, Asp::UBR_B, Asp::UBR_R, Asp::UBR_U, Asp::UBL_B, Asp::UBL_L, Asp::UBL_U, Asp::DBL_B, Asp::DBL_L, Asp::DBL_D, Asp::DFR_F, Asp::DFR_R, Asp::DFR_D, Asp::UFR_F, Asp::UFR_R, Asp::UFR_U, Asp::UFL_F, Asp::UFL_L, Asp::UFL_U, Asp::DFL_F, Asp::DFL_L, Asp::DFL_D },
		{ Asp::L, Asp::R, Asp::B, Asp::F, Asp::D, Asp::U, Asp::DB_B, Asp::DB_D, Asp::UB_B, Asp::UB_U, Asp::UF_F, Asp::UF_U, Asp::DF_F, Asp::DF_D, Asp::BL_L, Asp::BL_B, Asp::UL_L, Asp::UL_U, Asp::FL_L, Asp::FL_F, Asp::DL_L, Asp::DL_D, Asp::BR_R, Asp::BR_B, Asp::UR_R, Asp::UR_U, Asp::FR_R, Asp::FR_F, Asp::DR_R, Asp::DR_D, Asp::DBL_L, Asp::DBL_B, Asp::DBL_D, Asp::UBL_L, Asp::UBL_B, Asp::UBL_U, Asp::UFL_L, Asp::UFL_F, Asp::UFL_U, Asp::DFL_L, Asp::DFL_F, Asp::DFL_D, Asp::DBR_R, Asp::DBR_B, Asp::DBR_D, Asp::UBR_R, Asp::UBR_B, Asp::UBR_U, Asp::UFR_R, Asp::UFR_F, Asp::UFR_U, Asp::DFR_R, Asp::DFR_F, Asp::DFR_D }
	};
	
    void Cube::U() // Movement U
    {
        switch (spin)
        {
            case Spn::UF:
            case Spn::UR:
            case Spn::UB:
            case Spn::UL: e_U(); c_U(); return;
            case Spn::DF:
            case Spn::DR:
            case Spn::DB:
            case Spn::DL: e_D(); c_D(); return;
            case Spn::FU:
            case Spn::FR:
            case Spn::FD:
            case Spn::FL: e_F(); c_F(); return;
            case Spn::BU:
            case Spn::BR:
            case Spn::BD:
            case Spn::BL: e_B(); c_B(); return;
            case Spn::RU:
            case Spn::RF:
            case Spn::RD:
            case Spn::RB: e_R(); c_R(); return;
            case Spn::LU:
            case Spn::LF:
            case Spn::LD:
            case Spn::LB: e_L(); c_L(); return;
        }
    }
    
    void Cube::U2()  // Movement U2
    {
        switch (spin)
        {
            case Spn::UF:
            case Spn::UR:
            case Spn::UB:
            case Spn::UL: e_U2(); c_U2(); return;
            case Spn::DF:
            case Spn::DR:
            case Spn::DB:
            case Spn::DL: e_D2(); c_D2(); return;
            case Spn::FU:
            case Spn::FR:
            case Spn::FD:
            case Spn::FL: e_F2(); c_F2(); return;
            case Spn::BU:
            case Spn::BR:
            case Spn::BD:
            case Spn::BL: e_B2(); c_B2(); return;
            case Spn::RU:
            case Spn::RF:
            case Spn::RD:
            case Spn::RB: e_R2(); c_R2(); return;
            case Spn::LU:
            case Spn::LF:
            case Spn::LD:
            case Spn::LB: e_L2(); c_L2(); return;
        }
    }
    
    void Cube::Up()  // Movement U'
    {
        switch (spin)
        {
            case Spn::UF:
            case Spn::UR:
            case Spn::UB:
            case Spn::UL: e_Up(); c_Up(); return;
            case Spn::DF:
            case Spn::DR:
            case Spn::DB:
            case Spn::DL: e_Dp(); c_Dp(); return;
            case Spn::FU:
            case Spn::FR:
            case Spn::FD:
            case Spn::FL: e_Fp(); c_Fp(); return;
            case Spn::BU:
            case Spn::BR:
            case Spn::BD:
            case Spn::BL: e_Bp(); c_Bp(); return;
            case Spn::RU:
            case Spn::RF:
            case Spn::RD:
            case Spn::RB: e_Rp(); c_Rp(); return;
            case Spn::LU:
            case Spn::LF:
            case Spn::LD:
            case Spn::LB: e_Lp(); c_Lp(); return;
        }
    }
    
    void Cube::D()  // Movement D
    {
        switch (spin)
        {
            case Spn::UF:
            case Spn::UR:
            case Spn::UB:
            case Spn::UL: e_D(); c_D(); return;
            case Spn::DF:
            case Spn::DR:
            case Spn::DB:
            case Spn::DL: e_U(); c_U(); return;
            case Spn::FU:
            case Spn::FR:
            case Spn::FD:
            case Spn::FL: e_B(); c_B(); return;
            case Spn::BU:
            case Spn::BR:
            case Spn::BD:
            case Spn::BL: e_F(); c_F(); return;
            case Spn::RU:
            case Spn::RF:
            case Spn::RD:
            case Spn::RB: e_L(); c_L(); return;
            case Spn::LU:
            case Spn::LF:
            case Spn::LD:
            case Spn::LB: e_R(); c_R(); return;
        }
    }
    
    void Cube::D2()  // Movement D2
    {
        switch (spin)
        {
            case Spn::UF:
            case Spn::UR:
            case Spn::UB:
            case Spn::UL: e_D2(); c_D2(); return;
            case Spn::DF:
            case Spn::DR:
            case Spn::DB:
            case Spn::DL: e_U2(); c_U2(); return;
            case Spn::FU:
            case Spn::FR:
            case Spn::FD:
            case Spn::FL: e_B2(); c_B2(); return;
            case Spn::BU:
            case Spn::BR:
            case Spn::BD:
            case Spn::BL: e_F2(); c_F2(); return;
            case Spn::RU:
            case Spn::RF:
            case Spn::RD:
            case Spn::RB: e_L2(); c_L2(); return;
            case Spn::LU:
            case Spn::LF:
            case Spn::LD:
            case Spn::LB: e_R2(); c_R2(); return;
        }
    }
    
    void Cube::Dp()  // Movement D'
    {
        switch (spin)
        {
            case Spn::UF:
            case Spn::UR:
            case Spn::UB:
            case Spn::UL: e_Dp(); c_Dp(); return;
            case Spn::DF:
            case Spn::DR:
            case Spn::DB:
            case Spn::DL: e_Up(); c_Up(); return;
            case Spn::FU:
            case Spn::FR:
            case Spn::FD:
            case Spn::FL: e_Bp(); c_Bp(); return;
            case Spn::BU:
            case Spn::BR:
            case Spn::BD:
            case Spn::BL: e_Fp(); c_Fp(); return;
            case Spn::RU:
            case Spn::RF:
            case Spn::RD:
            case Spn::RB: e_Lp(); c_Lp(); return;
            case Spn::LU:
            case Spn::LF:
            case Spn::LD:
            case Spn::LB: e_Rp(); c_Rp(); return;
        }
    }
    
    void Cube::F()  // Movement F
    {
        switch (spin)
        {
            case Spn::FU:
            case Spn::BU:
            case Spn::RU:
            case Spn::LU: e_U(); c_U(); return;
            case Spn::FD:
            case Spn::BD:
            case Spn::RD:
            case Spn::LD: e_D(); c_D(); return;
            case Spn::UF:
            case Spn::DF:
            case Spn::RF:
            case Spn::LF: e_F(); c_F(); return;
            case Spn::UB:
            case Spn::DB:
            case Spn::RB:
            case Spn::LB: e_B(); c_B(); return;
            case Spn::UR:
            case Spn::DR:
            case Spn::FR:
            case Spn::BR: e_R(); c_R(); return;
            case Spn::UL:
            case Spn::DL:
            case Spn::FL:
            case Spn::BL: e_L(); c_L(); return;
        }
    }
    
    void Cube::F2()  // Movement F2
    {
        switch (spin)
        {
            case Spn::FU:
            case Spn::BU:
            case Spn::RU:
            case Spn::LU: e_U2(); c_U2(); return;
            case Spn::FD:
            case Spn::BD:
            case Spn::RD:
            case Spn::LD: e_D2(); c_D2(); return;
            case Spn::UF:
            case Spn::DF:
            case Spn::RF:
            case Spn::LF: e_F2(); c_F2(); return;
            case Spn::UB:
            case Spn::DB:
            case Spn::RB:
            case Spn::LB: e_B2(); c_B2(); return;
            case Spn::UR:
            case Spn::DR:
            case Spn::FR:
            case Spn::BR: e_R2(); c_R2(); return;
            case Spn::UL:
            case Spn::DL:
            case Spn::FL:
            case Spn::BL: e_L2(); c_L2(); return;
        }
    }
    
    void Cube::Fp()  // Movement F'
    {
        switch (spin)
        {
            case Spn::FU:
            case Spn::BU:
            case Spn::RU:
            case Spn::LU: e_Up(); c_Up(); return;
            case Spn::FD:
            case Spn::BD:
            case Spn::RD:
            case Spn::LD: e_Dp(); c_Dp(); return;
            case Spn::UF:
            case Spn::DF:
            case Spn::RF:
            case Spn::LF: e_Fp(); c_Fp(); return;
            case Spn::UB:
            case Spn::DB:
            case Spn::RB:
            case Spn::LB: e_Bp(); c_Bp(); return;
            case Spn::UR:
            case Spn::DR:
            case Spn::FR:
            case Spn::BR: e_Rp(); c_Rp(); return;
            case Spn::UL:
            case Spn::DL:
            case Spn::FL:
            case Spn::BL: e_Lp(); c_Lp(); return;
        }
    }
    
    void Cube::B()  // Movement B
    {
        switch (spin)
        {
            case Spn::FU:
            case Spn::BU:
            case Spn::RU:
            case Spn::LU: e_D(); c_D(); return;
            case Spn::FD:
            case Spn::BD:
            case Spn::RD:
            case Spn::LD: e_U(); c_U(); return;
            case Spn::UF:
            case Spn::DF:
            case Spn::RF:
            case Spn::LF: e_B(); c_B(); return;
            case Spn::UB:
            case Spn::DB:
            case Spn::RB:
            case Spn::LB: e_F(); c_F(); return;
            case Spn::UR:
            case Spn::DR:
            case Spn::FR:
            case Spn::BR: e_L(); c_L(); return;
            case Spn::UL:
            case Spn::DL:
            case Spn::FL:
            case Spn::BL: e_R(); c_R(); return;
        }
    }
    
    void Cube::B2()  // Movement B2
    {
        switch (spin)
        {
            case Spn::FU:
            case Spn::BU:
            case Spn::RU:
            case Spn::LU: e_D2(); c_D2(); return;
            case Spn::FD:
            case Spn::BD:
            case Spn::RD:
            case Spn::LD: e_U2(); c_U2(); return;
            case Spn::UF:
            case Spn::DF:
            case Spn::RF:
            case Spn::LF: e_B2(); c_B2(); return;
            case Spn::UB:
            case Spn::DB:
            case Spn::RB:
            case Spn::LB: e_F2(); c_F2(); return;
            case Spn::UR:
            case Spn::DR:
            case Spn::FR:
            case Spn::BR: e_L2(); c_L2(); return;
            case Spn::UL:
            case Spn::DL:
            case Spn::FL:
            case Spn::BL: e_R2(); c_R2(); return;
        }
    }
    
    void Cube::Bp()  // Movement B'
    {
        switch (spin)
        {
            case Spn::FU:
            case Spn::BU:
            case Spn::RU:
            case Spn::LU: e_Dp(); c_Dp(); return;
            case Spn::FD:
            case Spn::BD:
            case Spn::RD:
            case Spn::LD: e_Up(); c_Up(); return;
            case Spn::UF:
            case Spn::DF:
            case Spn::RF:
            case Spn::LF: e_Bp(); c_Bp(); return;
            case Spn::UB:
            case Spn::DB:
            case Spn::RB:
            case Spn::LB: e_Fp(); c_Fp(); return;
            case Spn::UR:
            case Spn::DR:
            case Spn::FR:
            case Spn::BR: e_Lp(); c_Lp(); return;
            case Spn::UL:
            case Spn::DL:
            case Spn::FL:
            case Spn::BL: e_Rp(); c_Rp(); return;
        }
    }
    
    void Cube::R()  // Movement R
    {
        switch (spin)
        {
            case Spn::FR:
            case Spn::BL:
            case Spn::RB:
            case Spn::LF: e_U(); c_U(); return;
            case Spn::FL:
            case Spn::BR:
            case Spn::RF:
            case Spn::LB: e_D(); c_D(); return;
            case Spn::UL:
            case Spn::DR:
            case Spn::RU:
            case Spn::LD: e_F(); c_F(); return;
            case Spn::UR:
            case Spn::DL:
            case Spn::RD:
            case Spn::LU: e_B(); c_B(); return;
            case Spn::UF:
            case Spn::DB:
            case Spn::FD:
            case Spn::BU: e_R(); c_R(); return;
            case Spn::UB:
            case Spn::DF:
            case Spn::FU:
            case Spn::BD: e_L(); c_L(); return;
        }
    }
    
    void Cube::R2()  // Movement R2
    {
        switch (spin)
        {
            case Spn::FR:
            case Spn::BL:
            case Spn::RB:
            case Spn::LF: e_U2(); c_U2(); return;
            case Spn::FL:
            case Spn::BR:
            case Spn::RF:
            case Spn::LB: e_D2(); c_D2(); return;
            case Spn::UL:
            case Spn::DR:
            case Spn::RU:
            case Spn::LD: e_F2(); c_F2(); return;
            case Spn::UR:
            case Spn::DL:
            case Spn::RD:
            case Spn::LU: e_B2(); c_B2(); return;
            case Spn::UF:
            case Spn::DB:
            case Spn::FD:
            case Spn::BU: e_R2(); c_R2(); return;
            case Spn::UB:
            case Spn::DF:
            case Spn::FU:
            case Spn::BD: e_L2(); c_L2(); return;
        }
    }
    
    void Cube::Rp()  // Movement R'
    {
        switch (spin)
        {
            case Spn::FR:
            case Spn::BL:
            case Spn::RB:
            case Spn::LF: e_Up(); c_Up(); return;
            case Spn::FL:
            case Spn::BR:
            case Spn::RF:
            case Spn::LB: e_Dp(); c_Dp(); return;
            case Spn::UL:
            case Spn::DR:
            case Spn::RU:
            case Spn::LD: e_Fp(); c_Fp(); return;
            case Spn::UR:
            case Spn::DL:
            case Spn::RD:
            case Spn::LU: e_Bp(); c_Bp(); return;
            case Spn::UF:
            case Spn::DB:
            case Spn::FD:
            case Spn::BU: e_Rp(); c_Rp(); return;
            case Spn::UB:
            case Spn::DF:
            case Spn::FU:
            case Spn::BD: e_Lp(); c_Lp(); return;
        }
    }
    
    void Cube::L()  // Movement L
    {
        switch (spin)
        {
            case Spn::FR:
            case Spn::BL:
            case Spn::RB:
            case Spn::LF: e_D(); c_D(); return;
            case Spn::FL:
            case Spn::BR:
            case Spn::RF:
            case Spn::LB: e_U(); c_U(); return;
            case Spn::UL:
            case Spn::DR:
            case Spn::RU:
            case Spn::LD: e_B(); c_B(); return;
            case Spn::UR:
            case Spn::DL:
            case Spn::RD:
            case Spn::LU: e_F(); c_F(); return;
            case Spn::UF:
            case Spn::DB:
            case Spn::FD:
            case Spn::BU: e_L(); c_L(); return;
            case Spn::UB:
            case Spn::DF:
            case Spn::FU:
            case Spn::BD: e_R(); c_R(); return;
        }
    }
    
    void Cube::L2()  // Movement L2
    {
        switch (spin)
        {
            case Spn::FR:
            case Spn::BL:
            case Spn::RB:
            case Spn::LF: e_D2(); c_D2(); return;
            case Spn::FL:
            case Spn::BR:
            case Spn::RF:
            case Spn::LB: e_U2(); c_U2(); return;
            case Spn::UL:
            case Spn::DR:
            case Spn::RU:
            case Spn::LD: e_B2(); c_B2(); return;
            case Spn::UR:
            case Spn::DL:
            case Spn::RD:
            case Spn::LU: e_F2(); c_F2(); return;
            case Spn::UF:
            case Spn::DB:
            case Spn::FD:
            case Spn::BU: e_L2(); c_L2(); return;
            case Spn::UB:
            case Spn::DF:
            case Spn::FU:
            case Spn::BD: e_R2(); c_R2(); return;
        }
    }
    
    void Cube::Lp()  // Movement L'
    {
        switch (spin)
        {
            case Spn::FR:
            case Spn::BL:
            case Spn::RB:
            case Spn::LF: e_Dp(); c_Dp(); return;
            case Spn::FL:
            case Spn::BR:
            case Spn::RF:
            case Spn::LB: e_Up(); c_Up(); return;
            case Spn::UL:
            case Spn::DR:
            case Spn::RU:
            case Spn::LD: e_Bp(); c_Bp(); return;
            case Spn::UR:
            case Spn::DL:
            case Spn::RD:
            case Spn::LU: e_Fp(); c_Fp(); return;
            case Spn::UF:
            case Spn::DB:
            case Spn::FD:
            case Spn::BU: e_Lp(); c_Lp(); return;
            case Spn::UB:
            case Spn::DF:
            case Spn::FU:
            case Spn::BD: e_Rp(); c_Rp(); return;
        }
    }
	
    // Set cube spin
    void Cube::SetSpin(const Spn s)
    {
        Stp T1, T2;
        GetSpinsSteps(spin, s, T1, T2);
        if (Algorithm::IsTurn(T1)) ApplyStep(T1);
        if (Algorithm::IsTurn(T2)) ApplyStep(T2);
    }

    // Apply algorithm to cube (algorithm should have developed parentheses)
    // If there are parentheses with more than one repetitions returns false and the cube will be in an undefined state!
    bool Cube::ApplyAlgorithm(const Algorithm& A)
    {
        // Apply movements (steps)
        for (uint n = 0u; n < A.GetSize(); n++) if(!ApplyStep(A.At(n))) return false;
        return true;
    }

    // Apply movement (step) to the cube
    // Returns false if the step is a close parentheses with more than one repetition
    bool Cube::ApplyStep(const Stp s)
    {
        switch (s)
        {
            case Stp::U:  U();  return true;
            case Stp::Up: Up(); return true;
            case Stp::U2: U2(); return true;
            case Stp::D:  D();  return true;
            case Stp::Dp: Dp(); return true;
            case Stp::D2: D2(); return true;
            case Stp::F:  F();  return true;
            case Stp::Fp: Fp(); return true;
            case Stp::F2: F2(); return true;
            case Stp::B:  B();  return true;
            case Stp::Bp: Bp(); return true;
            case Stp::B2: B2(); return true;
            case Stp::R:  R();  return true;
            case Stp::Rp: Rp(); return true;
            case Stp::R2: R2(); return true;
            case Stp::L:  L();  return true;
            case Stp::Lp: Lp(); return true;
            case Stp::L2: L2(); return true;
            
            case Stp::x:  x();  return true;
            case Stp::xp: xp(); return true;
            case Stp::x2: x2(); return true;
            case Stp::y:  y();  return true;
            case Stp::yp: yp(); return true;
            case Stp::y2: y2(); return true;
            case Stp::z:  z();  return true;
            case Stp::zp: zp(); return true;
            case Stp::z2: z2(); return true;

            case Stp::u:  D();  y();  return true;
            case Stp::up: Dp(); yp(); return true;
            case Stp::u2: D2(); y2(); return true;
            case Stp::d:  U();  yp(); return true;
            case Stp::dp: Up(); y();  return true;
            case Stp::d2: U2(); y2(); return true;
            case Stp::f:  B();  z();  return true;
            case Stp::fp: Bp(); zp(); return true;
            case Stp::f2: B2(); z2(); return true;
            case Stp::b:  F();  zp(); return true;
            case Stp::bp: Fp(); z();  return true;
            case Stp::b2: F2(); z2(); return true;
            case Stp::r:  L();  x();  return true;
            case Stp::rp: Lp(); xp(); return true;
            case Stp::r2: L2(); x2(); return true;
            case Stp::l:  R();  xp(); return true;
            case Stp::lp: Rp(); x();  return true;
            case Stp::l2: R2(); x2(); return true;

            case Stp::E:  U();  Dp(); yp(); return true; // E -> As layer D
            case Stp::Ep: Up(); D();  y();  return true; // E'
            case Stp::E2: U2(); D2(); y2(); return true; // E2
            case Stp::S:  Fp(); B();  z();  return true; // S -> As layer F
            case Stp::Sp: F();  Bp(); zp(); return true; // S'
            case Stp::S2: F2(); B2(); z2(); return true; // S2
            case Stp::M:  R();  Lp(); xp(); return true; // M -> As layer L
            case Stp::Mp: Rp(); L();  x();  return true; // M'
            case Stp::M2: R2(); L2(); x2(); return true; // M2

            case Stp::NONE:
            case Stp::PARENTHESIS_OPEN:
            case Stp::PARENTHESIS_CLOSE_1_REP: return true;

            default: return false; // Close parentheses step with more than one repetition
        }
    }
    
    // Get the sticker in the given sticker position
    Stc Cube::GetSticker(const Sps SP) const
    {
        switch (SP)
        {
        // Edges
        case Sps::FR_F: return stc_FR_F[GetStatusPos_FR()][GetStatusDir_FR()];
        case Sps::FR_R: return stc_FR_R[GetStatusPos_FR()][GetStatusDir_FR()];
        case Sps::FL_F: return stc_FL_F[GetStatusPos_FL()][GetStatusDir_FL()];
        case Sps::FL_L: return stc_FL_L[GetStatusPos_FL()][GetStatusDir_FL()];
        case Sps::BL_B: return stc_BL_B[GetStatusPos_BL()][GetStatusDir_BL()];
        case Sps::BL_L: return stc_BL_L[GetStatusPos_BL()][GetStatusDir_BL()];
        case Sps::BR_B: return stc_BR_B[GetStatusPos_BR()][GetStatusDir_BR()];
        case Sps::BR_R: return stc_BR_R[GetStatusPos_BR()][GetStatusDir_BR()];

        case Sps::UF_U: return stc_UF_U[GetStatusPos_UF()][GetStatusDir_UF()];
        case Sps::UF_F: return stc_UF_F[GetStatusPos_UF()][GetStatusDir_UF()];
        case Sps::UL_U: return stc_UL_U[GetStatusPos_UL()][GetStatusDir_UL()];
        case Sps::UL_L: return stc_UL_L[GetStatusPos_UL()][GetStatusDir_UL()];
        case Sps::UB_U: return stc_UB_U[GetStatusPos_UB()][GetStatusDir_UB()];
        case Sps::UB_B: return stc_UB_B[GetStatusPos_UB()][GetStatusDir_UB()];
        case Sps::UR_U: return stc_UR_U[GetStatusPos_UR()][GetStatusDir_UR()];
        case Sps::UR_R: return stc_UR_R[GetStatusPos_UR()][GetStatusDir_UR()];

        case Sps::DF_D: return stc_DF_D[GetStatusPos_DF()][GetStatusDir_DF()];
        case Sps::DF_F: return stc_DF_F[GetStatusPos_DF()][GetStatusDir_DF()];
        case Sps::DL_D: return stc_DL_D[GetStatusPos_DL()][GetStatusDir_DL()];
        case Sps::DL_L: return stc_DL_L[GetStatusPos_DL()][GetStatusDir_DL()];
        case Sps::DB_D: return stc_DB_D[GetStatusPos_DB()][GetStatusDir_DB()];
        case Sps::DB_B: return stc_DB_B[GetStatusPos_DB()][GetStatusDir_DB()];
        case Sps::DR_D: return stc_DR_D[GetStatusPos_DR()][GetStatusDir_DR()];
        case Sps::DR_R: return stc_DR_R[GetStatusPos_DR()][GetStatusDir_DR()];

        // Corners
        case Sps::UFR_U: return stc_UFR_U[GetStatusPos_UFR()][GetStatusDir_UFR()];
        case Sps::UFR_F: return stc_UFR_F[GetStatusPos_UFR()][GetStatusDir_UFR()];
        case Sps::UFR_R: return stc_UFR_R[GetStatusPos_UFR()][GetStatusDir_UFR()];
        case Sps::UFL_U: return stc_UFL_U[GetStatusPos_UFL()][GetStatusDir_UFL()];
        case Sps::UFL_F: return stc_UFL_F[GetStatusPos_UFL()][GetStatusDir_UFL()];
        case Sps::UFL_L: return stc_UFL_L[GetStatusPos_UFL()][GetStatusDir_UFL()];

        case Sps::UBL_U: return stc_UBL_U[GetStatusPos_UBL()][GetStatusDir_UBL()];
        case Sps::UBL_B: return stc_UBL_B[GetStatusPos_UBL()][GetStatusDir_UBL()];
        case Sps::UBL_L: return stc_UBL_L[GetStatusPos_UBL()][GetStatusDir_UBL()];
        case Sps::UBR_U: return stc_UBR_U[GetStatusPos_UBR()][GetStatusDir_UBR()];
        case Sps::UBR_B: return stc_UBR_B[GetStatusPos_UBR()][GetStatusDir_UBR()];
        case Sps::UBR_R: return stc_UBR_R[GetStatusPos_UBR()][GetStatusDir_UBR()];

        case Sps::DFR_D: return stc_DFR_D[GetStatusPos_DFR()][GetStatusDir_DFR()];
        case Sps::DFR_F: return stc_DFR_F[GetStatusPos_DFR()][GetStatusDir_DFR()];
        case Sps::DFR_R: return stc_DFR_R[GetStatusPos_DFR()][GetStatusDir_DFR()];
        case Sps::DFL_D: return stc_DFL_D[GetStatusPos_DFL()][GetStatusDir_DFL()];
        case Sps::DFL_F: return stc_DFL_F[GetStatusPos_DFL()][GetStatusDir_DFL()];
        case Sps::DFL_L: return stc_DFL_L[GetStatusPos_DFL()][GetStatusDir_DFL()];

        case Sps::DBL_D: return stc_DBL_D[GetStatusPos_DBL()][GetStatusDir_DBL()];
        case Sps::DBL_B: return stc_DBL_B[GetStatusPos_DBL()][GetStatusDir_DBL()];
        case Sps::DBL_L: return stc_DBL_L[GetStatusPos_DBL()][GetStatusDir_DBL()];
        case Sps::DBR_D: return stc_DBR_D[GetStatusPos_DBR()][GetStatusDir_DBR()];
        case Sps::DBR_B: return stc_DBR_B[GetStatusPos_DBR()][GetStatusDir_DBR()];
        case Sps::DBR_R: return stc_DBR_R[GetStatusPos_DBR()][GetStatusDir_DBR()];

        default: return static_cast<Stc>(SP); // Centers
        }
    }

    // Get the current piece in the given piece position
    Pce Cube::GetPiece(const Pcp P) const
    {
        switch (P)
        {
        // Edg
        case Pcp::UF: return static_cast<Pce>(static_cast<int>(GetEdge_UF()) + static_cast<int>(Pce::First_Edge));
        case Pcp::UB: return static_cast<Pce>(static_cast<int>(GetEdge_UB()) + static_cast<int>(Pce::First_Edge));
        case Pcp::UR: return static_cast<Pce>(static_cast<int>(GetEdge_UR()) + static_cast<int>(Pce::First_Edge));
        case Pcp::UL: return static_cast<Pce>(static_cast<int>(GetEdge_UL()) + static_cast<int>(Pce::First_Edge));

        case Pcp::DF: return static_cast<Pce>(static_cast<int>(GetEdge_DF()) + static_cast<int>(Pce::First_Edge));
        case Pcp::DB: return static_cast<Pce>(static_cast<int>(GetEdge_DB()) + static_cast<int>(Pce::First_Edge));
        case Pcp::DR: return static_cast<Pce>(static_cast<int>(GetEdge_DR()) + static_cast<int>(Pce::First_Edge));
        case Pcp::DL: return static_cast<Pce>(static_cast<int>(GetEdge_DL()) + static_cast<int>(Pce::First_Edge));

        case Pcp::FR: return static_cast<Pce>(static_cast<int>(GetEdge_FR()) + static_cast<int>(Pce::First_Edge));
        case Pcp::FL: return static_cast<Pce>(static_cast<int>(GetEdge_FL()) + static_cast<int>(Pce::First_Edge));
        case Pcp::BR: return static_cast<Pce>(static_cast<int>(GetEdge_BR()) + static_cast<int>(Pce::First_Edge));
        case Pcp::BL: return static_cast<Pce>(static_cast<int>(GetEdge_BL()) + static_cast<int>(Pce::First_Edge));

        // Cnr
        case Pcp::UFR: return static_cast<Pce>(static_cast<int>(GetCorner_UFR()) + static_cast<int>(Pce::First_Corner));
        case Pcp::UFL: return static_cast<Pce>(static_cast<int>(GetCorner_UFL()) + static_cast<int>(Pce::First_Corner));
        case Pcp::UBR: return static_cast<Pce>(static_cast<int>(GetCorner_UBR()) + static_cast<int>(Pce::First_Corner));
        case Pcp::UBL: return static_cast<Pce>(static_cast<int>(GetCorner_UBL()) + static_cast<int>(Pce::First_Corner));

        case Pcp::DFR: return static_cast<Pce>(static_cast<int>(GetCorner_DFR()) + static_cast<int>(Pce::First_Corner));
        case Pcp::DFL: return static_cast<Pce>(static_cast<int>(GetCorner_DFL()) + static_cast<int>(Pce::First_Corner));
        case Pcp::DBR: return static_cast<Pce>(static_cast<int>(GetCorner_DBR()) + static_cast<int>(Pce::First_Corner));
        case Pcp::DBL: return static_cast<Pce>(static_cast<int>(GetCorner_DBL()) + static_cast<int>(Pce::First_Corner));

        default: return static_cast<Pce>(P); // Centers
        }
    }

    // Get the current piece direction in the given piece position
    Pcd Cube::GetPieceDirection(const Pcp pp) const
    {
		if (IsEdge(pp)) return EddToPcd(GetEdgeDirection(PcpToEdp(pp)));
		if (IsCorner(pp)) return CndToPcd(GetCornerDirection(PcpToCnp(pp)));
        return Pcd::NONE; // Centers
    }
	
	// Get the piece orientation in the given piece position
    Pco Cube::GetPieceOrientation(const Pcp pp) const
	{
		if (IsEdge(pp))
		{
			Edp ep = PcpToEdp(pp);
			Edg e = GetEdge(ep);
			Edd ed = GetEdgeDirection(ep);
			if (ed == edd_OrientationPOS[static_cast<int>(spin)][static_cast<int>(ep)][static_cast<int>(e)]) return Pco::POS;
			return Pco::NEG;
		}
		
		if (IsCorner(pp))
		{
			Cnp cp = PcpToCnp(pp);
			Cnr c = GetCorner(cp);
			Cnd cd = GetCornerDirection(cp);
			if (cd == cnd_OrientationUD[static_cast<int>(spin)][static_cast<int>(cp)][static_cast<int>(c)]) return Pco::UD;
            if (cd == cnd_OrientationFB[static_cast<int>(spin)][static_cast<int>(cp)][static_cast<int>(c)]) return Pco::FB;
            if (cd == cnd_OrientationRL[static_cast<int>(spin)][static_cast<int>(cp)][static_cast<int>(c)]) return Pco::RL;
		}
		
		return Pco::NONE; // Centers
	}

    // Get the current position for the given piece
    Pcp Cube::GetPiecePosition(const Pce P) const
    {
        if (IsEdge(P)) // Edges
        {
            if (GetPiece(Pcp::UF) == P) return Pcp::UF;
            if (GetPiece(Pcp::UB) == P) return Pcp::UB;
            if (GetPiece(Pcp::UR) == P) return Pcp::UR;
            if (GetPiece(Pcp::UL) == P) return Pcp::UL;

            if (GetPiece(Pcp::DF) == P) return Pcp::DF;
            if (GetPiece(Pcp::DB) == P) return Pcp::DB;
            if (GetPiece(Pcp::DR) == P) return Pcp::DR;
            if (GetPiece(Pcp::DL) == P) return Pcp::DL;

            if (GetPiece(Pcp::FR) == P) return Pcp::FR;
            if (GetPiece(Pcp::FL) == P) return Pcp::FL;
            if (GetPiece(Pcp::BR) == P) return Pcp::BR;
            if (GetPiece(Pcp::BL) == P) return Pcp::BL;
        }

        if (IsCorner(P)) // Corners
        {
            if (GetPiece(Pcp::UFR) == P) return Pcp::UFR;
            if (GetPiece(Pcp::UFL) == P) return Pcp::UFL;
            if (GetPiece(Pcp::UBR) == P) return Pcp::UBR;
            if (GetPiece(Pcp::UBL) == P) return Pcp::UBL;

            if (GetPiece(Pcp::DFR) == P) return Pcp::DFR;
            if (GetPiece(Pcp::DFL) == P) return Pcp::DFL;
            if (GetPiece(Pcp::DBR) == P) return Pcp::DBR;
            if (GetPiece(Pcp::DBL) == P) return Pcp::DBL;
        }

        return static_cast<Pcp>(P); // Centers
    }
				    
    // Check a piece is in his given piece position (orientation not checked)
    bool Cube::CheckPosition(const Pcp pp) const
    {				
        if (IsEdge(pp)) return CheckEdgePosition(PcpToEdp(pp)); // Edges
        if (IsCorner(pp)) return CheckCornerPosition(PcpToCnp(pp)); // Corners
        return true; // Centers
    }

    // Check a group of pieces are in his given piece positions (direction not checked)
    bool Cube::CheckPosition(const Pgr PG) const
    {
        std::vector<Pcp> PList;
        AddToPositionsList(PG, PList);
        for (const auto PP : PList) if (!CheckPosition(PP)) return false;
        return true;
    }

    // Check a group of pieces are orientated
    bool Cube::CheckOrientation(const Pgr PG) const
    {
        std::vector<Pcp> PList;
        AddToPositionsList(PG, PList);
        for (const auto PP : PList) if (!CheckOrientation(PP)) return false;
        return true;
    }

    // Check if a list of pieces are orientated
    bool Cube::CheckOrientation(const std::vector<Pcp>& PList) const
    {
        for (const auto PP : PList) if (!CheckOrientation(PP)) return false;
        return true;
    }

    /* Edges oriented (http://cube.rider.biz/zz.php?p=eoline)
    Look at the U/D faces. If you see:

        L/R colour it's bad.
        F/B colour means you need to look round the side of the edge. If the side is U/D it is bad.

    Then look at the F/B faces of the E-slice (middle layer). The same rules apply. If you see:

        L/R colour it's bad.
        F/B colour means you need to look round the side of the edge. If the side is U/D it is bad. */
    bool Cube::EO_POS(const Edp ep) const // Edge in given edge position orientated
    {
        const Fce Fce_U_Slice = GetFace(FromAbsPosition(Asp::U, spin)),
                  Fce_D_Slice = GetFace(FromAbsPosition(Asp::D, spin)),
                  Fce_F_Slice = GetFace(FromAbsPosition(Asp::F, spin)),
                  Fce_B_Slice = GetFace(FromAbsPosition(Asp::B, spin)),
                  Fce_R_Slice = GetFace(FromAbsPosition(Asp::R, spin)),
                  Fce_L_Slice = GetFace(FromAbsPosition(Asp::L, spin));

        Asp MainStickerAPos, SecStickerAPos;
        switch (AbsFromPosition(EdpToPcp(ep), spin))
        {
        case App::UF: MainStickerAPos = Asp::UF_U; SecStickerAPos = Asp::UF_F; break;
        case App::UB: MainStickerAPos = Asp::UB_U; SecStickerAPos = Asp::UB_B; break;
        case App::UR: MainStickerAPos = Asp::UR_U; SecStickerAPos = Asp::UR_R; break;
        case App::UL: MainStickerAPos = Asp::UL_U; SecStickerAPos = Asp::UL_L; break;

        case App::DF: MainStickerAPos = Asp::DF_D; SecStickerAPos = Asp::DF_F; break;
        case App::DB: MainStickerAPos = Asp::DB_D; SecStickerAPos = Asp::DB_B; break;
        case App::DR: MainStickerAPos = Asp::DR_D; SecStickerAPos = Asp::DR_R; break;
        case App::DL: MainStickerAPos = Asp::DL_D; SecStickerAPos = Asp::DL_L; break;

        case App::FR: MainStickerAPos = Asp::FR_F; SecStickerAPos = Asp::FR_R; break;
        case App::FL: MainStickerAPos = Asp::FL_F; SecStickerAPos = Asp::FL_L; break;
        case App::BR: MainStickerAPos = Asp::BR_B; SecStickerAPos = Asp::BR_R; break;
        case App::BL: MainStickerAPos = Asp::BL_B; SecStickerAPos = Asp::BL_L; break;

        default: return false;
        }

        Fce Fce_Aux = GetFace(FromAbsPosition(MainStickerAPos, spin));
        if (Fce_Aux == Fce_R_Slice || Fce_Aux == Fce_L_Slice) return false;
        if (Fce_Aux == Fce_F_Slice || Fce_Aux == Fce_B_Slice)
        {
            Fce_Aux = GetFace(FromAbsPosition(SecStickerAPos, spin));
            if (Fce_Aux == Fce_U_Slice || Fce_Aux == Fce_D_Slice) return false;
        }
        return true;
    }

	// Edges oriented
    bool Cube::EO() const
    {
        for (int n = 0; n < 12; n++) if (!EO_POS(static_cast<Edp>(n))) return false;
        return true;
    }
    
    bool Cube::CO_UD(const Cnp cp) const // Corner in given corner position orientated UD
    {
        const Fce Fce_U_Slice = GetFace(FromAbsPosition(Asp::U, spin)),
                  Fce_D_Slice = GetFace(FromAbsPosition(Asp::D, spin));

        Asp MainStickerAPos;
        switch (AbsFromPosition(CnpToPcp(cp), spin))
        {
        case App::UFR: MainStickerAPos = Asp::UFR_U; break;
        case App::UFL: MainStickerAPos = Asp::UFL_U; break;
        case App::UBR: MainStickerAPos = Asp::UBR_U; break;
        case App::UBL: MainStickerAPos = Asp::UBL_U; break;

        case App::DFR: MainStickerAPos = Asp::DFR_D; break;
        case App::DFL: MainStickerAPos = Asp::DFL_D; break;
        case App::DBR: MainStickerAPos = Asp::DBR_D; break;
        case App::DBL: MainStickerAPos = Asp::DBL_D; break;

        default: return false;
        }
        Fce Fce_Aux = GetFace(FromAbsPosition(MainStickerAPos, spin));
        return Fce_Aux == Fce_U_Slice || Fce_Aux == Fce_D_Slice;
    }

    bool Cube::CO_FB(const Cnp cp) const // Corner in given corner position orientated FB
    {
        const Fce Fce_F_Slice = GetFace(FromAbsPosition(Asp::F, spin)),
                  Fce_B_Slice = GetFace(FromAbsPosition(Asp::B, spin));

        Asp MainStickerAPos;
        switch (AbsFromPosition(CnpToPcp(cp), spin))
        {
        case App::UFR: MainStickerAPos = Asp::UFR_U; break;
        case App::UFL: MainStickerAPos = Asp::UFL_U; break;
        case App::UBR: MainStickerAPos = Asp::UBR_U; break;
        case App::UBL: MainStickerAPos = Asp::UBL_U; break;

        case App::DFR: MainStickerAPos = Asp::DFR_D; break;
        case App::DFL: MainStickerAPos = Asp::DFL_D; break;
        case App::DBR: MainStickerAPos = Asp::DBR_D; break;
        case App::DBL: MainStickerAPos = Asp::DBL_D; break;

        default: return false;
        }
        Fce Fce_Aux = GetFace(FromAbsPosition(MainStickerAPos, spin));
        return Fce_Aux == Fce_F_Slice || Fce_Aux == Fce_B_Slice;
    }

    bool Cube::CO_RL(const Cnp cp) const // Corner in given corner position orientated RL
    {
        const Fce Fce_R_Slice = GetFace(FromAbsPosition(Asp::R, spin)),
                  Fce_L_Slice = GetFace(FromAbsPosition(Asp::L, spin));

        Asp MainStickerAPos;
        switch (AbsFromPosition(CnpToPcp(cp), spin))
        {
        case App::UFR: MainStickerAPos = Asp::UFR_U; break;
        case App::UFL: MainStickerAPos = Asp::UFL_U; break;
        case App::UBR: MainStickerAPos = Asp::UBR_U; break;
        case App::UBL: MainStickerAPos = Asp::UBL_U; break;

        case App::DFR: MainStickerAPos = Asp::DFR_D; break;
        case App::DFL: MainStickerAPos = Asp::DFL_D; break;
        case App::DBR: MainStickerAPos = Asp::DBR_D; break;
        case App::DBL: MainStickerAPos = Asp::DBL_D; break;

        default: return false;
        }
        Fce Fce_Aux = GetFace(FromAbsPosition(MainStickerAPos, spin));
        return Fce_Aux == Fce_R_Slice || Fce_Aux == Fce_L_Slice;
    }

	// Corners oriented
    bool Cube::CO() const
    {
        for (int n = 0; n < 8; n++) if (!CO_UD(static_cast<Cnp>(n))) return false;
        return true;
    }

    // Check a piece orientation
	/* Edges oriented (http://cube.rider.biz/zz.php?p=eoline)
    Look at the U/D faces. If you see:

        L/R colour it's bad.
        F/B colour means you need to look round the side of the edge. If the side is U/D it is bad.

    Then look at the F/B faces of the E-slice (middle layer). The same rules apply. If you see:

        L/R colour it's bad.
        F/B colour means you need to look round the side of the edge. If the side is U/D it is bad. */
    bool Cube::CheckOrientation(const Pcp pp) const
    {
		if (IsEdge(pp))
		{
			const Edp ep = PcpToEdp(pp);
			const Edg e = GetEdge(ep);
			return GetEdgeDirection(ep) == edd_OrientationPOS[static_cast<int>(spin)][static_cast<int>(ep)][static_cast<int>(e)];
		}
		
		if (IsCorner(pp))
		{
			const Cnp cp = PcpToCnp(pp);
			const Cnr c = GetCorner(cp);
			return GetCornerDirection(cp) == cnd_OrientationUD[static_cast<int>(spin)][static_cast<int>(cp)][static_cast<int>(c)];
		}
			
        return true; // Centers
    }
	
	// Don't use, slower than CheckOrientation
	bool Cube::CheckOrientation2(const Pcp pp) const
    {
        if (IsEdge(pp)) return EO_POS(PcpToEdp(pp)); // Edges
        if (IsCorner(pp)) return CO_UD(PcpToCnp(pp)); // Corners
        return true; // Centers
    }

    // Get the sticker face in the given sticker position
    Fce Cube::GetFace(const Sps SP) const
    {
        switch (SP)
        {
        // Edges
        case Sps::FR_F: return fce_FR_F[GetStatusPos_FR()][GetStatusDir_FR()];
        case Sps::FR_R: return fce_FR_R[GetStatusPos_FR()][GetStatusDir_FR()];
        case Sps::FL_F: return fce_FL_F[GetStatusPos_FL()][GetStatusDir_FL()];
        case Sps::FL_L: return fce_FL_L[GetStatusPos_FL()][GetStatusDir_FL()];
        case Sps::BL_B: return fce_BL_B[GetStatusPos_BL()][GetStatusDir_BL()];
        case Sps::BL_L: return fce_BL_L[GetStatusPos_BL()][GetStatusDir_BL()];
        case Sps::BR_B: return fce_BR_B[GetStatusPos_BR()][GetStatusDir_BR()];
        case Sps::BR_R: return fce_BR_R[GetStatusPos_BR()][GetStatusDir_BR()];

        case Sps::UF_U: return fce_UF_U[GetStatusPos_UF()][GetStatusDir_UF()];
        case Sps::UF_F: return fce_UF_F[GetStatusPos_UF()][GetStatusDir_UF()];
        case Sps::UL_U: return fce_UL_U[GetStatusPos_UL()][GetStatusDir_UL()];
        case Sps::UL_L: return fce_UL_L[GetStatusPos_UL()][GetStatusDir_UL()];
        case Sps::UB_U: return fce_UB_U[GetStatusPos_UB()][GetStatusDir_UB()];
        case Sps::UB_B: return fce_UB_B[GetStatusPos_UB()][GetStatusDir_UB()];
        case Sps::UR_U: return fce_UR_U[GetStatusPos_UR()][GetStatusDir_UR()];
        case Sps::UR_R: return fce_UR_R[GetStatusPos_UR()][GetStatusDir_UR()];

        case Sps::DF_D: return fce_DF_D[GetStatusPos_DF()][GetStatusDir_DF()];
        case Sps::DF_F: return fce_DF_F[GetStatusPos_DF()][GetStatusDir_DF()];
        case Sps::DL_D: return fce_DL_D[GetStatusPos_DL()][GetStatusDir_DL()];
        case Sps::DL_L: return fce_DL_L[GetStatusPos_DL()][GetStatusDir_DL()];
        case Sps::DB_D: return fce_DB_D[GetStatusPos_DB()][GetStatusDir_DB()];
        case Sps::DB_B: return fce_DB_B[GetStatusPos_DB()][GetStatusDir_DB()];
        case Sps::DR_D: return fce_DR_D[GetStatusPos_DR()][GetStatusDir_DR()];
        case Sps::DR_R: return fce_DR_R[GetStatusPos_DR()][GetStatusDir_DR()];

        // Corners
        case Sps::UFR_U: return fce_UFR_U[GetStatusPos_UFR()][GetStatusDir_UFR()];
        case Sps::UFR_F: return fce_UFR_F[GetStatusPos_UFR()][GetStatusDir_UFR()];
        case Sps::UFR_R: return fce_UFR_R[GetStatusPos_UFR()][GetStatusDir_UFR()];
        case Sps::UFL_U: return fce_UFL_U[GetStatusPos_UFL()][GetStatusDir_UFL()];
        case Sps::UFL_F: return fce_UFL_F[GetStatusPos_UFL()][GetStatusDir_UFL()];
        case Sps::UFL_L: return fce_UFL_L[GetStatusPos_UFL()][GetStatusDir_UFL()];

        case Sps::UBL_U: return fce_UBL_U[GetStatusPos_UBL()][GetStatusDir_UBL()];
        case Sps::UBL_B: return fce_UBL_B[GetStatusPos_UBL()][GetStatusDir_UBL()];
        case Sps::UBL_L: return fce_UBL_L[GetStatusPos_UBL()][GetStatusDir_UBL()];
        case Sps::UBR_U: return fce_UBR_U[GetStatusPos_UBR()][GetStatusDir_UBR()];
        case Sps::UBR_B: return fce_UBR_B[GetStatusPos_UBR()][GetStatusDir_UBR()];
        case Sps::UBR_R: return fce_UBR_R[GetStatusPos_UBR()][GetStatusDir_UBR()];

        case Sps::DFR_D: return fce_DFR_D[GetStatusPos_DFR()][GetStatusDir_DFR()];
        case Sps::DFR_F: return fce_DFR_F[GetStatusPos_DFR()][GetStatusDir_DFR()];
        case Sps::DFR_R: return fce_DFR_R[GetStatusPos_DFR()][GetStatusDir_DFR()];
        case Sps::DFL_D: return fce_DFL_D[GetStatusPos_DFL()][GetStatusDir_DFL()];
        case Sps::DFL_F: return fce_DFL_F[GetStatusPos_DFL()][GetStatusDir_DFL()];
        case Sps::DFL_L: return fce_DFL_L[GetStatusPos_DFL()][GetStatusDir_DFL()];

        case Sps::DBL_D: return fce_DBL_D[GetStatusPos_DBL()][GetStatusDir_DBL()];
        case Sps::DBL_B: return fce_DBL_B[GetStatusPos_DBL()][GetStatusDir_DBL()];
        case Sps::DBL_L: return fce_DBL_L[GetStatusPos_DBL()][GetStatusDir_DBL()];
        case Sps::DBR_D: return fce_DBR_D[GetStatusPos_DBR()][GetStatusDir_DBR()];
        case Sps::DBR_B: return fce_DBR_B[GetStatusPos_DBR()][GetStatusDir_DBR()];
        case Sps::DBR_R: return fce_DBR_R[GetStatusPos_DBR()][GetStatusDir_DBR()];

        default: return static_cast<Fce>(SP); // Centers
        }
    }

    // Check if given piece is in given layer
    bool Cube::IsPieceInLayer(const Pce P, const Lyr Ly) const
    {
        if (IsCorner(P)) return IsCornerInLayer(PceToCnr(P), Ly);
        if (IsEdge(P)) return IsEdgeInLayer(PceToEdg(P), Ly);

        return ((P == Pce::U && Ly == Lyr::U) || (P == Pce::D && Ly == Lyr::D) ||
                (P == Pce::F && Ly == Lyr::F) || (P == Pce::B && Ly == Lyr::B) ||
                (P == Pce::R && Ly == Lyr::R) || (P == Pce::L && Ly == Lyr::L));
    }

    // Check if given pieces are in given layer
    bool Cube::ArePiecesInLayer(const std::vector<Pce>& Ps, const Lyr Ly) const
    {
        for (const auto P : Ps) if (!IsPieceInLayer(P, Ly)) return false;
        return true;
    }

    // Add groups of pieces positions to a pieces positions list
    bool Cube::AddToPositionsList(const std::vector<Pgr>& SPV, std::vector<Pcp>& PList)
    {
        for (const auto SP : SPV) { if (!AddToPositionsList(SP, PList)) return false; }
        return true;
    }

    // Add a group of pieces positions to a pieces positions list
    bool Cube::AddToPositionsList(const Pgr SP, std::vector<Pcp>& PList)
    {
        switch (SP)
        {
		// All cube pieces
		case Pgr::ALL:
            for (int i = static_cast<int>(Pcp::First_Edge); i <= static_cast<int>(Pcp::Last); i++)
                PList.push_back(static_cast<Pcp>(i));
            return true;

        // All cube edges
        case Pgr::ALL_EDGES:
            for (int i = static_cast<int>(Pcp::First_Edge); i <= static_cast<int>(Pcp::Last_Edge); i++)
                PList.push_back(static_cast<Pcp>(i));
            return true;

        // All cube corners
        case Pgr::ALL_CORNERS:
            for (int i = static_cast<int>(Pcp::First_Corner); i <= static_cast<int>(Pcp::Last_Corner); i++)
                PList.push_back(static_cast<Pcp>(i));
            return true;

        // All pieces in each layer
        case Pgr::LAYER_U: AddToPositionsList(Pgr::CROSS_U, PList);
                           AddToPositionsList(Pgr::CORNERS_U, PList); return true;
        case Pgr::LAYER_D: AddToPositionsList(Pgr::CROSS_D, PList);
                           AddToPositionsList(Pgr::CORNERS_D, PList); return true;
        case Pgr::LAYER_F: AddToPositionsList(Pgr::CROSS_F, PList);
                           AddToPositionsList(Pgr::CORNERS_F, PList); return true;
        case Pgr::LAYER_B: AddToPositionsList(Pgr::CROSS_B, PList);
                           AddToPositionsList(Pgr::CORNERS_B, PList); return true;
        case Pgr::LAYER_R: AddToPositionsList(Pgr::CROSS_R, PList);
                           AddToPositionsList(Pgr::CORNERS_R, PList); return true;
        case Pgr::LAYER_L: AddToPositionsList(Pgr::CROSS_L, PList);
                           AddToPositionsList(Pgr::CORNERS_L, PList); return true;
                                                                
        case Pgr::LAYER_E:
            PList.push_back(Pcp::FR); PList.push_back(Pcp::FL);
            PList.push_back(Pcp::BR); PList.push_back(Pcp::BL); return true;
        case Pgr::LAYER_S:
            PList.push_back(Pcp::UR); PList.push_back(Pcp::UL);
            PList.push_back(Pcp::DR); PList.push_back(Pcp::DL); return true;
        case Pgr::LAYER_M:
            PList.push_back(Pcp::UF); PList.push_back(Pcp::UB);
            PList.push_back(Pcp::DF); PList.push_back(Pcp::DB); return true;
																		
        // Four Edg in a cross
        case Pgr::CROSS_U:
            PList.push_back(Pcp::UF); PList.push_back(Pcp::UB);
            PList.push_back(Pcp::UR); PList.push_back(Pcp::UL); return true;
        case Pgr::CROSS_D:
            PList.push_back(Pcp::DF); PList.push_back(Pcp::DB);
            PList.push_back(Pcp::DR); PList.push_back(Pcp::DL); return true;
        case Pgr::CROSS_F:
            PList.push_back(Pcp::UF); PList.push_back(Pcp::DF);
            PList.push_back(Pcp::FR); PList.push_back(Pcp::FL); return true;
        case Pgr::CROSS_B:
            PList.push_back(Pcp::UB); PList.push_back(Pcp::DB);
            PList.push_back(Pcp::BR); PList.push_back(Pcp::BL); return true;
        case Pgr::CROSS_R:
            PList.push_back(Pcp::UR); PList.push_back(Pcp::DR);
            PList.push_back(Pcp::FR); PList.push_back(Pcp::BR); return true;
        case Pgr::CROSS_L:
            PList.push_back(Pcp::UL); PList.push_back(Pcp::DL);
            PList.push_back(Pcp::FL); PList.push_back(Pcp::BL); return true;
						
        // All pieces in first two layers
        case Pgr::F2L_U: AddToPositionsList(Pgr::LAYER_U, PList);
                                AddToPositionsList(Pgr::LAYER_E, PList); return true;
        case Pgr::F2L_D: AddToPositionsList(Pgr::LAYER_D, PList);
                                AddToPositionsList(Pgr::LAYER_E, PList); return true;
        case Pgr::F2L_F: AddToPositionsList(Pgr::LAYER_F, PList);
                                AddToPositionsList(Pgr::LAYER_S, PList); return true;
        case Pgr::F2L_B: AddToPositionsList(Pgr::LAYER_B, PList);
                                AddToPositionsList(Pgr::LAYER_S, PList); return true;
        case Pgr::F2L_R: AddToPositionsList(Pgr::LAYER_R, PList);
                                AddToPositionsList(Pgr::LAYER_M, PList); return true;
        case Pgr::F2L_L: AddToPositionsList(Pgr::LAYER_L, PList);
                                AddToPositionsList(Pgr::LAYER_M, PList); return true;

        // Corner + Edge: First element must be a corner, second an edge
        case Pgr::F2L_U_UFR: PList.push_back(Pcp::UFR); PList.push_back(Pcp::FR); return true;
        case Pgr::F2L_U_UFL: PList.push_back(Pcp::UFL); PList.push_back(Pcp::FL); return true;
        case Pgr::F2L_U_UBR: PList.push_back(Pcp::UBR); PList.push_back(Pcp::BR); return true;
        case Pgr::F2L_U_UBL: PList.push_back(Pcp::UBL); PList.push_back(Pcp::BL); return true;

        case Pgr::F2L_D_DFR: PList.push_back(Pcp::DFR); PList.push_back(Pcp::FR); return true;
        case Pgr::F2L_D_DFL: PList.push_back(Pcp::DFL); PList.push_back(Pcp::FL); return true;
        case Pgr::F2L_D_DBR: PList.push_back(Pcp::DBR); PList.push_back(Pcp::BR); return true;
        case Pgr::F2L_D_DBL: PList.push_back(Pcp::DBL); PList.push_back(Pcp::BL); return true;

        case Pgr::F2L_F_UFR: PList.push_back(Pcp::UFR); PList.push_back(Pcp::UR); return true;
        case Pgr::F2L_F_UFL: PList.push_back(Pcp::UFL); PList.push_back(Pcp::UL); return true;
        case Pgr::F2L_F_DFR: PList.push_back(Pcp::DFR); PList.push_back(Pcp::DR); return true;
        case Pgr::F2L_F_DFL: PList.push_back(Pcp::DFL); PList.push_back(Pcp::DL); return true;

        case Pgr::F2L_B_UBR: PList.push_back(Pcp::UBR); PList.push_back(Pcp::UR); return true;
        case Pgr::F2L_B_UBL: PList.push_back(Pcp::UBL); PList.push_back(Pcp::UL); return true;
        case Pgr::F2L_B_DBR: PList.push_back(Pcp::DBR); PList.push_back(Pcp::DR); return true;
        case Pgr::F2L_B_DBL: PList.push_back(Pcp::DBL); PList.push_back(Pcp::DL); return true;

        case Pgr::F2L_R_UFR: PList.push_back(Pcp::UFR); PList.push_back(Pcp::UF); return true;
        case Pgr::F2L_R_UBR: PList.push_back(Pcp::UBR); PList.push_back(Pcp::UB); return true;
        case Pgr::F2L_R_DFR: PList.push_back(Pcp::DFR); PList.push_back(Pcp::DF); return true;
        case Pgr::F2L_R_DBR: PList.push_back(Pcp::DBR); PList.push_back(Pcp::DB); return true;

        case Pgr::F2L_L_UFL: PList.push_back(Pcp::UFL); PList.push_back(Pcp::UF); return true;
        case Pgr::F2L_L_UBL: PList.push_back(Pcp::UBL); PList.push_back(Pcp::UB); return true;
        case Pgr::F2L_L_DFL: PList.push_back(Pcp::DFL); PList.push_back(Pcp::DF); return true;
        case Pgr::F2L_L_DBL: PList.push_back(Pcp::DBL); PList.push_back(Pcp::DB); return true;

        // Firsts blocks for Roux method (B1 -> Left block / B2 -> Right block)
        case Pgr::UF_B2: // U face up
        case Pgr::UB_B1: PList.push_back(Pcp::DR); AddToPositionsList(Pgr::F2L_D_DFR, PList);
                                  AddToPositionsList(Pgr::F2L_D_DBR, PList); return true;
        case Pgr::UF_B1:
        case Pgr::UB_B2: PList.push_back(Pcp::DL); AddToPositionsList(Pgr::F2L_D_DFL, PList);
                                  AddToPositionsList(Pgr::F2L_D_DBL, PList); return true;
        case Pgr::UR_B2:
        case Pgr::UL_B1: PList.push_back(Pcp::DB); AddToPositionsList(Pgr::F2L_D_DBR, PList);
                                  AddToPositionsList(Pgr::F2L_D_DBL, PList); return true;
        case Pgr::UR_B1:
        case Pgr::UL_B2: PList.push_back(Pcp::DF); AddToPositionsList(Pgr::F2L_D_DFR, PList);
                                  AddToPositionsList(Pgr::F2L_D_DFL, PList); return true;

        case Pgr::DF_B2: // D face up
        case Pgr::DB_B1: PList.push_back(Pcp::UL); AddToPositionsList(Pgr::F2L_U_UFL, PList);
                                  AddToPositionsList(Pgr::F2L_U_UBL, PList); return true;
        case Pgr::DF_B1:
        case Pgr::DB_B2: PList.push_back(Pcp::UR); AddToPositionsList(Pgr::F2L_U_UFR, PList);
                                  AddToPositionsList(Pgr::F2L_U_UBR, PList); return true;
        case Pgr::DR_B2:
        case Pgr::DL_B1: PList.push_back(Pcp::UF); AddToPositionsList(Pgr::F2L_U_UFR, PList);
                                  AddToPositionsList(Pgr::F2L_U_UFL, PList); return true;
        case Pgr::DR_B1:
        case Pgr::DL_B2: PList.push_back(Pcp::UB); AddToPositionsList(Pgr::F2L_U_UBR, PList);
                                  AddToPositionsList(Pgr::F2L_U_UBL, PList); return true;

        case Pgr::FU_B2: // F face up
        case Pgr::FD_B1: PList.push_back(Pcp::BL); AddToPositionsList(Pgr::F2L_B_UBL, PList);
                                  AddToPositionsList(Pgr::F2L_B_DBL, PList); return true;
        case Pgr::FU_B1:
        case Pgr::FD_B2: PList.push_back(Pcp::BR); AddToPositionsList(Pgr::F2L_B_UBR, PList);
                                  AddToPositionsList(Pgr::F2L_B_DBR, PList); return true;
        case Pgr::FR_B2:
        case Pgr::FL_B1: PList.push_back(Pcp::UB); AddToPositionsList(Pgr::F2L_B_UBR, PList);
                                  AddToPositionsList(Pgr::F2L_B_UBL, PList); return true;
        case Pgr::FR_B1:
        case Pgr::FL_B2: PList.push_back(Pcp::DB); AddToPositionsList(Pgr::F2L_B_DBR, PList);
                                  AddToPositionsList(Pgr::F2L_B_DBL, PList); return true;

        case Pgr::BU_B2: // B face up
        case Pgr::BD_B1: PList.push_back(Pcp::FR); AddToPositionsList(Pgr::F2L_F_UFR, PList);
                                  AddToPositionsList(Pgr::F2L_F_DFR, PList); return true;
        case Pgr::BU_B1:
        case Pgr::BD_B2: PList.push_back(Pcp::FL); AddToPositionsList(Pgr::F2L_F_UFL, PList);
                                  AddToPositionsList(Pgr::F2L_F_DFL, PList); return true;
        case Pgr::BR_B2:
        case Pgr::BL_B1: PList.push_back(Pcp::DF); AddToPositionsList(Pgr::F2L_F_DFR, PList);
                                  AddToPositionsList(Pgr::F2L_F_DFL, PList); return true;
        case Pgr::BR_B1:
        case Pgr::BL_B2: PList.push_back(Pcp::UF); AddToPositionsList(Pgr::F2L_F_UFR, PList);
                                  AddToPositionsList(Pgr::F2L_F_UFL, PList); return true;

        case Pgr::RU_B2: // R face up
        case Pgr::RD_B1: PList.push_back(Pcp::FL); AddToPositionsList(Pgr::F2L_L_UFL, PList);
                                  AddToPositionsList(Pgr::F2L_L_DFL, PList); return true;
        case Pgr::RU_B1:
        case Pgr::RD_B2: PList.push_back(Pcp::BL); AddToPositionsList(Pgr::F2L_L_UBL, PList);
                                  AddToPositionsList(Pgr::F2L_L_DBL, PList); return true;
        case Pgr::RF_B2:
        case Pgr::RB_B1: PList.push_back(Pcp::DL); AddToPositionsList(Pgr::F2L_L_DFL, PList);
                                  AddToPositionsList(Pgr::F2L_L_DBL, PList); return true;
        case Pgr::RF_B1:
        case Pgr::RB_B2: PList.push_back(Pcp::UL); AddToPositionsList(Pgr::F2L_L_UFL, PList);
                                  AddToPositionsList(Pgr::F2L_L_UBL, PList); return true;

        case Pgr::LU_B2: // L face up
        case Pgr::LD_B1: PList.push_back(Pcp::BR); AddToPositionsList(Pgr::F2L_R_UBR, PList);
                                  AddToPositionsList(Pgr::F2L_R_DBR, PList); return true;
        case Pgr::LU_B1:
        case Pgr::LD_B2: PList.push_back(Pcp::FR); AddToPositionsList(Pgr::F2L_R_UFR, PList);
                                  AddToPositionsList(Pgr::F2L_R_DFR, PList); return true;
        case Pgr::LF_B2:
        case Pgr::LB_B1: PList.push_back(Pcp::UR); AddToPositionsList(Pgr::F2L_R_UFR, PList);
                                  AddToPositionsList(Pgr::F2L_R_UBR, PList); return true;
        case Pgr::LF_B1:
        case Pgr::LB_B2: PList.push_back(Pcp::DR); AddToPositionsList(Pgr::F2L_R_DFR, PList);
                                  AddToPositionsList(Pgr::F2L_R_DBR, PList); return true;

        case Pgr::UF_B1S1:
        case Pgr::UB_B2S2: PList.push_back(Pcp::DL); AddToPositionsList(Pgr::F2L_D_DFL, PList); return true;
        case Pgr::UF_B1S2:
        case Pgr::UB_B2S1: PList.push_back(Pcp::DL); AddToPositionsList(Pgr::F2L_D_DBL, PList); return true;
        case Pgr::UB_B1S1:
        case Pgr::UF_B2S2: PList.push_back(Pcp::DR); AddToPositionsList(Pgr::F2L_D_DBR, PList); return true;
        case Pgr::UB_B1S2:
        case Pgr::UF_B2S1: PList.push_back(Pcp::DR); AddToPositionsList(Pgr::F2L_D_DFR, PList); return true;
        case Pgr::UR_B1S1:
        case Pgr::UL_B2S2: PList.push_back(Pcp::DF); AddToPositionsList(Pgr::F2L_D_DFR, PList); return true;
        case Pgr::UR_B1S2:
        case Pgr::UL_B2S1: PList.push_back(Pcp::DF); AddToPositionsList(Pgr::F2L_D_DFL, PList); return true;
        case Pgr::UL_B1S1:
        case Pgr::UR_B2S2: PList.push_back(Pcp::DB); AddToPositionsList(Pgr::F2L_D_DBL, PList); return true;
        case Pgr::UL_B1S2:
        case Pgr::UR_B2S1: PList.push_back(Pcp::DB); AddToPositionsList(Pgr::F2L_D_DBR, PList); return true;

        case Pgr::DF_B1S1:
        case Pgr::DB_B2S2: PList.push_back(Pcp::UR); AddToPositionsList(Pgr::F2L_U_UFR, PList); return true;
        case Pgr::DF_B1S2:
        case Pgr::DB_B2S1: PList.push_back(Pcp::UR); AddToPositionsList(Pgr::F2L_U_UBR, PList); return true;
        case Pgr::DB_B1S1:
        case Pgr::DF_B2S2: PList.push_back(Pcp::UL); AddToPositionsList(Pgr::F2L_U_UBL, PList); return true;
        case Pgr::DB_B1S2:
        case Pgr::DF_B2S1: PList.push_back(Pcp::UL); AddToPositionsList(Pgr::F2L_U_UFL, PList); return true;
        case Pgr::DR_B1S1:
        case Pgr::DL_B2S2: PList.push_back(Pcp::UB); AddToPositionsList(Pgr::F2L_U_UBR, PList); return true;
        case Pgr::DR_B1S2:
        case Pgr::DL_B2S1: PList.push_back(Pcp::UB); AddToPositionsList(Pgr::F2L_U_UBL, PList); return true;
        case Pgr::DL_B1S1:
        case Pgr::DR_B2S2: PList.push_back(Pcp::UF); AddToPositionsList(Pgr::F2L_U_UFL, PList); return true;
        case Pgr::DL_B1S2:
        case Pgr::DR_B2S1: PList.push_back(Pcp::UF); AddToPositionsList(Pgr::F2L_U_UFR, PList); return true;

        case Pgr::FU_B1S1:
        case Pgr::FD_B2S2: PList.push_back(Pcp::BR); AddToPositionsList(Pgr::F2L_B_UBR, PList); return true;
        case Pgr::FU_B1S2:
        case Pgr::FD_B2S1: PList.push_back(Pcp::BR); AddToPositionsList(Pgr::F2L_B_DBR, PList); return true;
        case Pgr::FD_B1S1:
        case Pgr::FU_B2S2: PList.push_back(Pcp::BL); AddToPositionsList(Pgr::F2L_B_DBL, PList); return true;
        case Pgr::FD_B1S2:
        case Pgr::FU_B2S1: PList.push_back(Pcp::BL); AddToPositionsList(Pgr::F2L_B_UBL, PList); return true;
        case Pgr::FR_B1S1:
        case Pgr::FL_B2S2: PList.push_back(Pcp::DB); AddToPositionsList(Pgr::F2L_B_DBR, PList); return true;
        case Pgr::FR_B1S2:
        case Pgr::FL_B2S1: PList.push_back(Pcp::DB); AddToPositionsList(Pgr::F2L_B_DBL, PList); return true;
        case Pgr::FL_B1S1:
        case Pgr::FR_B2S2: PList.push_back(Pcp::UB); AddToPositionsList(Pgr::F2L_B_UBL, PList); return true;
        case Pgr::FL_B1S2:
        case Pgr::FR_B2S1: PList.push_back(Pcp::UB); AddToPositionsList(Pgr::F2L_B_UBR, PList); return true;

        case Pgr::BU_B1S1:
        case Pgr::BD_B2S2: PList.push_back(Pcp::FL); AddToPositionsList(Pgr::F2L_F_UFL, PList); return true;
        case Pgr::BU_B1S2:
        case Pgr::BD_B2S1: PList.push_back(Pcp::FL); AddToPositionsList(Pgr::F2L_F_DFL, PList); return true;
        case Pgr::BD_B1S1:
        case Pgr::BU_B2S2: PList.push_back(Pcp::FR); AddToPositionsList(Pgr::F2L_F_DFR, PList); return true;
        case Pgr::BD_B1S2:
        case Pgr::BU_B2S1: PList.push_back(Pcp::FR); AddToPositionsList(Pgr::F2L_F_UFR, PList); return true;
        case Pgr::BR_B1S1:
        case Pgr::BL_B2S2: PList.push_back(Pcp::UF); AddToPositionsList(Pgr::F2L_F_UFR, PList); return true;
        case Pgr::BR_B1S2:
        case Pgr::BL_B2S1: PList.push_back(Pcp::UF); AddToPositionsList(Pgr::F2L_F_UFL, PList); return true;
        case Pgr::BL_B1S1:
        case Pgr::BR_B2S2: PList.push_back(Pcp::DF); AddToPositionsList(Pgr::F2L_F_DFL, PList); return true;
        case Pgr::BL_B1S2:
        case Pgr::BR_B2S1: PList.push_back(Pcp::DF); AddToPositionsList(Pgr::F2L_F_DFR, PList); return true;

        case Pgr::RU_B1S1:
        case Pgr::RD_B2S2: PList.push_back(Pcp::BL); AddToPositionsList(Pgr::F2L_L_UBL, PList); return true;
        case Pgr::RU_B1S2:
        case Pgr::RD_B2S1: PList.push_back(Pcp::BL); AddToPositionsList(Pgr::F2L_L_DBL, PList); return true;
        case Pgr::RD_B1S1:
        case Pgr::RU_B2S2: PList.push_back(Pcp::FL); AddToPositionsList(Pgr::F2L_L_DFL, PList); return true;
        case Pgr::RD_B1S2:
        case Pgr::RU_B2S1: PList.push_back(Pcp::FL); AddToPositionsList(Pgr::F2L_L_UFL, PList); return true;
        case Pgr::RF_B1S1:
        case Pgr::RB_B2S2: PList.push_back(Pcp::UL); AddToPositionsList(Pgr::F2L_L_UFL, PList); return true;
        case Pgr::RF_B1S2:
        case Pgr::RB_B2S1: PList.push_back(Pcp::UL); AddToPositionsList(Pgr::F2L_L_UBL, PList); return true;
        case Pgr::RB_B1S1:
        case Pgr::RF_B2S2: PList.push_back(Pcp::DL); AddToPositionsList(Pgr::F2L_L_DBL, PList); return true;
        case Pgr::RB_B1S2:
        case Pgr::RF_B2S1: PList.push_back(Pcp::DL); AddToPositionsList(Pgr::F2L_L_DFL, PList); return true;

        case Pgr::LU_B1S1:
        case Pgr::LD_B2S2: PList.push_back(Pcp::FR); AddToPositionsList(Pgr::F2L_R_UFR, PList); return true;
        case Pgr::LU_B1S2:
        case Pgr::LD_B2S1: PList.push_back(Pcp::FR); AddToPositionsList(Pgr::F2L_R_DFR, PList); return true;
        case Pgr::LD_B1S1:
        case Pgr::LU_B2S2: PList.push_back(Pcp::BR); AddToPositionsList(Pgr::F2L_R_DBR, PList); return true;
        case Pgr::LD_B1S2:
        case Pgr::LU_B2S1: PList.push_back(Pcp::BR); AddToPositionsList(Pgr::F2L_R_UBR, PList); return true;
        case Pgr::LF_B1S1:
        case Pgr::LB_B2S2: PList.push_back(Pcp::DR); AddToPositionsList(Pgr::F2L_R_DFR, PList); return true;
        case Pgr::LF_B1S2:
        case Pgr::LB_B2S1: PList.push_back(Pcp::DR); AddToPositionsList(Pgr::F2L_R_DBR, PList); return true;
        case Pgr::LB_B1S1:
        case Pgr::LF_B2S2: PList.push_back(Pcp::UR); AddToPositionsList(Pgr::F2L_R_UBR, PList); return true;
        case Pgr::LB_B1S2:
        case Pgr::LF_B2S1: PList.push_back(Pcp::UR); AddToPositionsList(Pgr::F2L_R_UFR, PList); return true;

        case Pgr::CORNERS_U: PList.push_back(Pcp::UFR); PList.push_back(Pcp::UFL);
                                      PList.push_back(Pcp::UBR); PList.push_back(Pcp::UBL); return true;
        case Pgr::CORNERS_D: PList.push_back(Pcp::DFR); PList.push_back(Pcp::DFL);
                                      PList.push_back(Pcp::DBR); PList.push_back(Pcp::DBL); return true;
        case Pgr::CORNERS_F: PList.push_back(Pcp::UFR); PList.push_back(Pcp::UFL);
                                      PList.push_back(Pcp::DFR); PList.push_back(Pcp::DFL); return true;
        case Pgr::CORNERS_B: PList.push_back(Pcp::UBR); PList.push_back(Pcp::UBL);
                                      PList.push_back(Pcp::DBR); PList.push_back(Pcp::DBL); return true;
        case Pgr::CORNERS_R: PList.push_back(Pcp::UFR); PList.push_back(Pcp::UBR);
                                      PList.push_back(Pcp::DFR); PList.push_back(Pcp::DBR); return true;
        case Pgr::CORNERS_L: PList.push_back(Pcp::UFL); PList.push_back(Pcp::UBL);
                                      PList.push_back(Pcp::DFL); PList.push_back(Pcp::DBL); return true;

        // Roux last six edges
        case Pgr::ROUX_L6E_UF:
        case Pgr::ROUX_L6E_UB:
            AddToPositionsList(Pgr::CROSS_U, PList);
            PList.push_back(Pcp::DF); PList.push_back(Pcp::DB); return true;
        case Pgr::ROUX_L6E_UR:
        case Pgr::ROUX_L6E_UL:
            AddToPositionsList(Pgr::CROSS_U, PList);
            PList.push_back(Pcp::DR); PList.push_back(Pcp::DL); return true;
        case Pgr::ROUX_L6E_DF:
        case Pgr::ROUX_L6E_DB:
            AddToPositionsList(Pgr::CROSS_D, PList);
            PList.push_back(Pcp::UF); PList.push_back(Pcp::UB); return true;
        case Pgr::ROUX_L6E_DR:
        case Pgr::ROUX_L6E_DL:
            AddToPositionsList(Pgr::CROSS_D, PList);
            PList.push_back(Pcp::UR); PList.push_back(Pcp::UL); return true;

        case Pgr::ROUX_L6E_FU:
        case Pgr::ROUX_L6E_FD:
            AddToPositionsList(Pgr::CROSS_F, PList);
            PList.push_back(Pcp::UB); PList.push_back(Pcp::DB); return true;
        case Pgr::ROUX_L6E_FR:
        case Pgr::ROUX_L6E_FL:
            AddToPositionsList(Pgr::CROSS_F, PList);
            PList.push_back(Pcp::BR); PList.push_back(Pcp::BL); return true;
        case Pgr::ROUX_L6E_BU:
        case Pgr::ROUX_L6E_BD:
            AddToPositionsList(Pgr::CROSS_B, PList);
            PList.push_back(Pcp::UF); PList.push_back(Pcp::DF); return true;
        case Pgr::ROUX_L6E_BR:
        case Pgr::ROUX_L6E_BL:
            AddToPositionsList(Pgr::CROSS_B, PList);
            PList.push_back(Pcp::FR); PList.push_back(Pcp::FL); return true;

        case Pgr::ROUX_L6E_RU:
        case Pgr::ROUX_L6E_RD:
            AddToPositionsList(Pgr::CROSS_R, PList);
            PList.push_back(Pcp::UL); PList.push_back(Pcp::DL); return true;
        case Pgr::ROUX_L6E_RF:
        case Pgr::ROUX_L6E_RB:
            AddToPositionsList(Pgr::CROSS_R, PList);
            PList.push_back(Pcp::FL); PList.push_back(Pcp::BL); return true;
        case Pgr::ROUX_L6E_LU:
        case Pgr::ROUX_L6E_LD:
            AddToPositionsList(Pgr::CROSS_L, PList);
            PList.push_back(Pcp::UR); PList.push_back(Pcp::DR); return true;
        case Pgr::ROUX_L6E_LF:
        case Pgr::ROUX_L6E_LB:
            AddToPositionsList(Pgr::CROSS_L, PList);
            PList.push_back(Pcp::FR); PList.push_back(Pcp::BR); return true;

        // Petrus blocks
        case Pgr::PETRUS_UFR:
            PList.push_back(Pcp::UFR); PList.push_back(Pcp::UF);
            PList.push_back(Pcp::UR); PList.push_back(Pcp::FR); return true;
        case Pgr::PETRUS_UFL:
            PList.push_back(Pcp::UFL); PList.push_back(Pcp::UF);
            PList.push_back(Pcp::UL); PList.push_back(Pcp::FL); return true;
        case Pgr::PETRUS_UBR:
            PList.push_back(Pcp::UBR); PList.push_back(Pcp::UB);
            PList.push_back(Pcp::UR); PList.push_back(Pcp::BR); return true;
        case Pgr::PETRUS_UBL:
            PList.push_back(Pcp::UBL); PList.push_back(Pcp::UB);
            PList.push_back(Pcp::UL); PList.push_back(Pcp::BL); return true;
        case Pgr::PETRUS_DFR:
            PList.push_back(Pcp::DFR); PList.push_back(Pcp::DF);
            PList.push_back(Pcp::DR); PList.push_back(Pcp::FR); return true;
        case Pgr::PETRUS_DFL:
            PList.push_back(Pcp::DFL); PList.push_back(Pcp::DF);
            PList.push_back(Pcp::DL); PList.push_back(Pcp::FL); return true;
        case Pgr::PETRUS_DBR:
            PList.push_back(Pcp::DBR); PList.push_back(Pcp::DB);
            PList.push_back(Pcp::DR); PList.push_back(Pcp::BR); return true;
        case Pgr::PETRUS_DBL:
            PList.push_back(Pcp::DBL); PList.push_back(Pcp::DB);
            PList.push_back(Pcp::DL); PList.push_back(Pcp::BL); return true;

        case Pgr::PETRUS_UFR_X:
        case Pgr::PETRUS_UFL_X: AddToPositionsList(Pgr::PETRUS_UFR, PList);
            PList.push_back(Pcp::UFL); PList.push_back(Pcp::UL);
            PList.push_back(Pcp::FL); return true;
        case Pgr::PETRUS_UBR_X:
        case Pgr::PETRUS_UBL_X: AddToPositionsList(Pgr::PETRUS_UBR, PList);
            PList.push_back(Pcp::UBL); PList.push_back(Pcp::UL);
            PList.push_back(Pcp::BL); return true;
        case Pgr::PETRUS_DFR_X:
        case Pgr::PETRUS_DFL_X: AddToPositionsList(Pgr::PETRUS_DFR, PList);
            PList.push_back(Pcp::DFL); PList.push_back(Pcp::DL);
            PList.push_back(Pcp::FL); return true;
        case Pgr::PETRUS_DBR_X:
        case Pgr::PETRUS_DBL_X: AddToPositionsList(Pgr::PETRUS_DBR, PList);
            PList.push_back(Pcp::DBL); PList.push_back(Pcp::DL);
            PList.push_back(Pcp::BL); return true;

        case Pgr::PETRUS_UFR_Y:
        case Pgr::PETRUS_DFR_Y: AddToPositionsList(Pgr::PETRUS_UFR, PList);
            PList.push_back(Pcp::DFR); PList.push_back(Pcp::DF);
            PList.push_back(Pcp::DR); return true;
        case Pgr::PETRUS_UFL_Y:
        case Pgr::PETRUS_DFL_Y: AddToPositionsList(Pgr::PETRUS_UFL, PList);
            PList.push_back(Pcp::DFL); PList.push_back(Pcp::DF);
            PList.push_back(Pcp::DL); return true;
        case Pgr::PETRUS_UBR_Y:
        case Pgr::PETRUS_DBR_Y: AddToPositionsList(Pgr::PETRUS_UBR, PList);
            PList.push_back(Pcp::DBR); PList.push_back(Pcp::DB);
            PList.push_back(Pcp::DR); return true;
        case Pgr::PETRUS_UBL_Y:
        case Pgr::PETRUS_DBL_Y: AddToPositionsList(Pgr::PETRUS_UBL, PList);
            PList.push_back(Pcp::DBL); PList.push_back(Pcp::DB);
            PList.push_back(Pcp::DL); return true;

        case Pgr::PETRUS_UFR_Z:
        case Pgr::PETRUS_UBR_Z: AddToPositionsList(Pgr::PETRUS_UFR, PList);
            PList.push_back(Pcp::UBR); PList.push_back(Pcp::UB);
            PList.push_back(Pcp::BR); return true;
        case Pgr::PETRUS_UFL_Z:
        case Pgr::PETRUS_UBL_Z: AddToPositionsList(Pgr::PETRUS_UFL, PList);
            PList.push_back(Pcp::UBL); PList.push_back(Pcp::UB);
            PList.push_back(Pcp::BL); return true;
        case Pgr::PETRUS_DFR_Z:
        case Pgr::PETRUS_DBR_Z: AddToPositionsList(Pgr::PETRUS_DFR, PList);
            PList.push_back(Pcp::DBR); PList.push_back(Pcp::DB);
            PList.push_back(Pcp::BR); return true;
        case Pgr::PETRUS_DFL_Z:
        case Pgr::PETRUS_DBL_Z: AddToPositionsList(Pgr::PETRUS_DFL, PList);
            PList.push_back(Pcp::DBL); PList.push_back(Pcp::DB);
            PList.push_back(Pcp::BL); return true;
            
        // ZZ EO Lines
        case Pgr::EOLINE_UF:
        case Pgr::EOLINE_UB: PList.push_back(Pcp::DF); PList.push_back(Pcp::DB); return true;
        case Pgr::EOLINE_UR:
        case Pgr::EOLINE_UL: PList.push_back(Pcp::DR); PList.push_back(Pcp::DL); return true;
        case Pgr::EOLINE_DF:
        case Pgr::EOLINE_DB: PList.push_back(Pcp::UF); PList.push_back(Pcp::UB); return true;
        case Pgr::EOLINE_DR:
        case Pgr::EOLINE_DL: PList.push_back(Pcp::UR); PList.push_back(Pcp::UL); return true;
             
        case Pgr::EOLINE_FU:
        case Pgr::EOLINE_FD: PList.push_back(Pcp::UB); PList.push_back(Pcp::DB); return true;
        case Pgr::EOLINE_FR:
        case Pgr::EOLINE_FL: PList.push_back(Pcp::BR); PList.push_back(Pcp::BL); return true;
        case Pgr::EOLINE_BU:
        case Pgr::EOLINE_BD: PList.push_back(Pcp::UF); PList.push_back(Pcp::DF); return true;
        case Pgr::EOLINE_BR:
        case Pgr::EOLINE_BL: PList.push_back(Pcp::FR); PList.push_back(Pcp::FL); return true;
             
        case Pgr::EOLINE_RU:
        case Pgr::EOLINE_RD: PList.push_back(Pcp::UL); PList.push_back(Pcp::DL); return true;
        case Pgr::EOLINE_RF:
        case Pgr::EOLINE_RB: PList.push_back(Pcp::FL); PList.push_back(Pcp::BL); return true;
        case Pgr::EOLINE_LU:
        case Pgr::EOLINE_LD: PList.push_back(Pcp::UR); PList.push_back(Pcp::DR); return true;
        case Pgr::EOLINE_LF:
        case Pgr::EOLINE_LB: PList.push_back(Pcp::FR); PList.push_back(Pcp::BR); return true;

        // yruRU CP Lines
        case Pgr::CPLINE_UF:
        case Pgr::CPLINE_RB: PList.push_back(Pcp::DFL); PList.push_back(Pcp::DBL); PList.push_back(Pcp::DL); return true;
        case Pgr::CPLINE_UB:
        case Pgr::CPLINE_LF: PList.push_back(Pcp::DBR); PList.push_back(Pcp::DFR); PList.push_back(Pcp::DR); return true;
        case Pgr::CPLINE_UR:
        case Pgr::CPLINE_BL: PList.push_back(Pcp::DFR); PList.push_back(Pcp::DFL); PList.push_back(Pcp::DF); return true;
        case Pgr::CPLINE_UL:
        case Pgr::CPLINE_FR: PList.push_back(Pcp::DBL); PList.push_back(Pcp::DBR); PList.push_back(Pcp::DB); return true;

        case Pgr::CPLINE_DF:
        case Pgr::CPLINE_LB: PList.push_back(Pcp::UFR); PList.push_back(Pcp::UBR); PList.push_back(Pcp::UR); return true;
        case Pgr::CPLINE_DB:
        case Pgr::CPLINE_RF: PList.push_back(Pcp::UBL); PList.push_back(Pcp::UFL); PList.push_back(Pcp::UL); return true;
        case Pgr::CPLINE_DR:
        case Pgr::CPLINE_FL: PList.push_back(Pcp::UBR); PList.push_back(Pcp::UBL); PList.push_back(Pcp::UB); return true;
        case Pgr::CPLINE_DL:
        case Pgr::CPLINE_BR: PList.push_back(Pcp::UFL); PList.push_back(Pcp::UFR); PList.push_back(Pcp::UF); return true;

        case Pgr::CPLINE_FU:
        case Pgr::CPLINE_LD: PList.push_back(Pcp::UBR); PList.push_back(Pcp::DBR); PList.push_back(Pcp::BR); return true;
        case Pgr::CPLINE_FD:
        case Pgr::CPLINE_RU: PList.push_back(Pcp::DBL); PList.push_back(Pcp::UBL); PList.push_back(Pcp::BL); return true;
        case Pgr::CPLINE_RD:
        case Pgr::CPLINE_BU: PList.push_back(Pcp::DFL); PList.push_back(Pcp::UFL); PList.push_back(Pcp::FL); return true;
        case Pgr::CPLINE_LU:
        case Pgr::CPLINE_BD: PList.push_back(Pcp::DFR); PList.push_back(Pcp::UFR); PList.push_back(Pcp::FR); return true;

        // Mehta three edge belts
        case Pgr::BELT_UD_1: PList.push_back(Pcp::FR); PList.push_back(Pcp::FL); PList.push_back(Pcp::BR); return true;
        case Pgr::BELT_UD_2: PList.push_back(Pcp::FR); PList.push_back(Pcp::FL); PList.push_back(Pcp::BL); return true;
        case Pgr::BELT_UD_3: PList.push_back(Pcp::FR); PList.push_back(Pcp::BR); PList.push_back(Pcp::BL); return true;
        case Pgr::BELT_UD_4: PList.push_back(Pcp::FL); PList.push_back(Pcp::BR); PList.push_back(Pcp::BL); return true;

        case Pgr::BELT_FB_1: PList.push_back(Pcp::UR); PList.push_back(Pcp::UL); PList.push_back(Pcp::DR); return true;
        case Pgr::BELT_FB_2: PList.push_back(Pcp::UR); PList.push_back(Pcp::UL); PList.push_back(Pcp::DL); return true;
        case Pgr::BELT_FB_3: PList.push_back(Pcp::UR); PList.push_back(Pcp::DR); PList.push_back(Pcp::DL); return true;
        case Pgr::BELT_FB_4: PList.push_back(Pcp::UL); PList.push_back(Pcp::DR); PList.push_back(Pcp::DL); return true;

        case Pgr::BELT_RL_1: PList.push_back(Pcp::UF); PList.push_back(Pcp::UB); PList.push_back(Pcp::DF); return true;
        case Pgr::BELT_RL_2: PList.push_back(Pcp::UF); PList.push_back(Pcp::UB); PList.push_back(Pcp::DB); return true;
        case Pgr::BELT_RL_3: PList.push_back(Pcp::UF); PList.push_back(Pcp::DF); PList.push_back(Pcp::DB); return true;
        case Pgr::BELT_RL_4: PList.push_back(Pcp::UB); PList.push_back(Pcp::DF); PList.push_back(Pcp::DB); return true;

        default: return false;
        }
    }

    // Check if the given piece cube is solved
    bool Cube::IsSolved(const Pce P) const
    {
        switch (P)
        {
        case Pce::FR: return IsSolved_FR();
        case Pce::FL: return IsSolved_FL();
        case Pce::BL: return IsSolved_BL();
        case Pce::BR: return IsSolved_BR();
        case Pce::UF: return IsSolved_UF();
        case Pce::UL: return IsSolved_UL();
        case Pce::UB: return IsSolved_UB();
        case Pce::UR: return IsSolved_UR();
        case Pce::DF: return IsSolved_DF();
        case Pce::DL: return IsSolved_DL();
        case Pce::DB: return IsSolved_DB();
        case Pce::DR: return IsSolved_DR();

        case Pce::UFR: return IsSolved_UFR();
        case Pce::UFL: return IsSolved_UFL();
        case Pce::UBL: return IsSolved_UBL();
        case Pce::UBR: return IsSolved_UBR();
        case Pce::DFR: return IsSolved_DFR();
        case Pce::DFL: return IsSolved_DFL();
        case Pce::DBL: return IsSolved_DBL();
        case Pce::DBR: return IsSolved_DBR();

        default: return true; // Centers
        }
    }

    // Check if the cube is solved for the given group of pieces
    bool Cube::IsSolved(const Pgr SP) const
    {
        std::vector<Pcp> PList;
        std::vector<Edp> EList;
        std::vector<Cnp> CList;

        AddToPositionsList(SP, PList);
        for (const auto P : PList)
        {
            if (IsEdge(P)) EList.push_back(PcpToEdp(P));
            else if (IsCorner(P)) CList.push_back(PcpToCnp(P));
        }

        return IsSolved(GetMaskSolvedEdges(EList), GetMaskSolvedCorners(CList));
    }

    // Check if the cube is solved for the given groups of pieces
    bool Cube::IsSolved(const std::vector<Pgr>& SPV) const
    {
        for (const auto SP : SPV) { if (!IsSolved(SP)) return false; }
        return true;
    }

    // Check if some piece positions are in solve condition
	bool Cube::IsSolved(const std::vector<Pcp>& SPV) const
    {
        for (const auto SP : SPV) { if (!IsSolved(SP)) return false; }
        return true;
    }

    // Check if the cube is solved for the given layer
    bool Cube::IsSolved(const Lyr Ly) const
    {
        switch (Ly)
        {
        case Lyr::U: return IsSolved(EMASK_U, CMASK_U);
        case Lyr::D: return IsSolved(EMASK_D, CMASK_D);
        case Lyr::F: return IsSolved(EMASK_F, CMASK_F);
        case Lyr::B: return IsSolved(EMASK_B, CMASK_B);
        case Lyr::R: return IsSolved(EMASK_R, CMASK_R);
        case Lyr::L: return IsSolved(EMASK_L, CMASK_L);

        case Lyr::E: return IsSolved(EMASK_E, CMASK_E);
        case Lyr::M: return IsSolved(EMASK_M, CMASK_M);
        case Lyr::S: return IsSolved(EMASK_S, CMASK_S);
        
        default: return false;
        }
    }

    // Checks is the main face of given layer has the correct orientation
    bool Cube::IsFaceOriented(const Lyr Ly) const
    {
        switch (Ly)
        {
        case Lyr::U: 
            return GetFace(Sps::UF_U) == Fce::U &&
                   GetFace(Sps::UB_U) == Fce::U &&
                   GetFace(Sps::UL_U) == Fce::U &&
                   GetFace(Sps::UR_U) == Fce::U &&
                   GetFace(Sps::UFL_U) == Fce::U &&
                   GetFace(Sps::UFR_U) == Fce::U &&
                   GetFace(Sps::UBL_U) == Fce::U &&
                   GetFace(Sps::UBR_U) == Fce::U;
        case Lyr::D: 
            return GetFace(Sps::DF_D) == Fce::D &&
                   GetFace(Sps::DB_D) == Fce::D &&
                   GetFace(Sps::DL_D) == Fce::D &&
                   GetFace(Sps::DR_D) == Fce::D &&
                   GetFace(Sps::DFL_D) == Fce::D &&
                   GetFace(Sps::DFR_D) == Fce::D &&
                   GetFace(Sps::DBL_D) == Fce::D &&
                   GetFace(Sps::DBR_D) == Fce::D;
        case Lyr::F: 
            return GetFace(Sps::UF_F) == Fce::F &&
                   GetFace(Sps::DF_F) == Fce::F &&
                   GetFace(Sps::FR_F) == Fce::F &&
                   GetFace(Sps::FL_F) == Fce::F &&
                   GetFace(Sps::UFR_F) == Fce::F &&
                   GetFace(Sps::UFL_F) == Fce::F &&
                   GetFace(Sps::DFR_F) == Fce::F &&
                   GetFace(Sps::DFL_F) == Fce::F;
        case Lyr::B: 
            return GetFace(Sps::UB_B) == Fce::B &&
                   GetFace(Sps::DB_B) == Fce::B &&
                   GetFace(Sps::BR_B) == Fce::B &&
                   GetFace(Sps::BL_B) == Fce::B &&
                   GetFace(Sps::UBR_B) == Fce::B &&
                   GetFace(Sps::UBL_B) == Fce::B &&
                   GetFace(Sps::DBR_B) == Fce::B &&
                   GetFace(Sps::DBL_B) == Fce::B;
        case Lyr::R: 
            return GetFace(Sps::UR_R) == Fce::R &&
                   GetFace(Sps::DR_R) == Fce::R &&
                   GetFace(Sps::FR_R) == Fce::R &&
                   GetFace(Sps::BR_R) == Fce::R &&
                   GetFace(Sps::UFR_R) == Fce::R &&
                   GetFace(Sps::UBR_R) == Fce::R &&
                   GetFace(Sps::DFR_R) == Fce::R &&
                   GetFace(Sps::DBR_R) == Fce::R;
        case Lyr::L: 
            return GetFace(Sps::UL_L) == Fce::L &&
                   GetFace(Sps::DL_L) == Fce::L &&
                   GetFace(Sps::FL_L) == Fce::L &&
                   GetFace(Sps::BL_L) == Fce::L &&
                   GetFace(Sps::UFL_L) == Fce::L &&
                   GetFace(Sps::UBL_L) == Fce::L &&
                   GetFace(Sps::DFL_L) == Fce::L &&
                   GetFace(Sps::DBL_L) == Fce::L;
        default: return false;
        }
    }
		
    // Checks is the main face of given layer has his face or the opposite
    bool Cube::IsFaceOriented2(const Lyr Ly) const
    {
        switch (Ly)
        {
        case Lyr::U: 
            return (GetFace(Sps::UF_U) == Fce::U || GetFace(Sps::UF_U) == Fce::D) &&
                   (GetFace(Sps::UB_U) == Fce::U || GetFace(Sps::UB_U) == Fce::D) &&
                   (GetFace(Sps::UL_U) == Fce::U || GetFace(Sps::UL_U) == Fce::D) &&
                   (GetFace(Sps::UR_U) == Fce::U || GetFace(Sps::UR_U) == Fce::D) &&
                   (GetFace(Sps::UFL_U) == Fce::U || GetFace(Sps::UFL_U) == Fce::D) &&
                   (GetFace(Sps::UFR_U) == Fce::U || GetFace(Sps::UFR_U) == Fce::D) &&
                   (GetFace(Sps::UBL_U) == Fce::U || GetFace(Sps::UBL_U) == Fce::D) &&
                   (GetFace(Sps::UBR_U) == Fce::U || GetFace(Sps::UBR_U) == Fce::D);
        case Lyr::D: 
            return (GetFace(Sps::DF_D) == Fce::D || GetFace(Sps::DF_D) == Fce::U) &&
                   (GetFace(Sps::DB_D) == Fce::D || GetFace(Sps::DB_D) == Fce::U) &&
                   (GetFace(Sps::DL_D) == Fce::D || GetFace(Sps::DL_D) == Fce::U) &&
                   (GetFace(Sps::DR_D) == Fce::D || GetFace(Sps::DR_D) == Fce::U) &&
                   (GetFace(Sps::DFL_D) == Fce::D || GetFace(Sps::DFL_D) == Fce::U) &&
                   (GetFace(Sps::DFR_D) == Fce::D || GetFace(Sps::DFR_D) == Fce::U) &&
                   (GetFace(Sps::DBL_D) == Fce::D || GetFace(Sps::DBL_D) == Fce::U) &&
                   (GetFace(Sps::DBR_D) == Fce::D || GetFace(Sps::DBR_D) == Fce::U);
        case Lyr::F: 
            return (GetFace(Sps::UF_F) == Fce::F || GetFace(Sps::UF_F) == Fce::B) &&
                   (GetFace(Sps::DF_F) == Fce::F || GetFace(Sps::DF_F) == Fce::B) &&
                   (GetFace(Sps::FR_F) == Fce::F || GetFace(Sps::FR_F) == Fce::B) &&
                   (GetFace(Sps::FL_F) == Fce::F || GetFace(Sps::FL_F) == Fce::B) &&
                   (GetFace(Sps::UFR_F) == Fce::F || GetFace(Sps::UFR_F) == Fce::B) &&
                   (GetFace(Sps::UFL_F) == Fce::F || GetFace(Sps::UFL_F) == Fce::B) &&
                   (GetFace(Sps::DFR_F) == Fce::F || GetFace(Sps::DFR_F) == Fce::B) &&
                   (GetFace(Sps::DFL_F) == Fce::F || GetFace(Sps::DFL_F) == Fce::B);
        case Lyr::B: 
            return (GetFace(Sps::UB_B) == Fce::B || GetFace(Sps::UB_B) == Fce::F) &&
                   (GetFace(Sps::DB_B) == Fce::B || GetFace(Sps::DB_B) == Fce::F) &&
                   (GetFace(Sps::BR_B) == Fce::B || GetFace(Sps::BR_B) == Fce::F) &&
                   (GetFace(Sps::BL_B) == Fce::B || GetFace(Sps::BL_B) == Fce::F) &&
                   (GetFace(Sps::UBR_B) == Fce::B || GetFace(Sps::UBR_B) == Fce::F) &&
                   (GetFace(Sps::UBL_B) == Fce::B || GetFace(Sps::UBL_B) == Fce::F) &&
                   (GetFace(Sps::DBR_B) == Fce::B || GetFace(Sps::DBR_B) == Fce::F) &&
                   (GetFace(Sps::DBL_B) == Fce::B || GetFace(Sps::DBL_B) == Fce::F);
        case Lyr::R: 
            return (GetFace(Sps::UR_R) == Fce::R || GetFace(Sps::UR_R) == Fce::L) &&
                   (GetFace(Sps::DR_R) == Fce::R || GetFace(Sps::DR_R) == Fce::L) &&
                   (GetFace(Sps::FR_R) == Fce::R || GetFace(Sps::FR_R) == Fce::L) &&
                   (GetFace(Sps::BR_R) == Fce::R || GetFace(Sps::BR_R) == Fce::L) &&
                   (GetFace(Sps::UFR_R) == Fce::R || GetFace(Sps::UFR_R) == Fce::L) &&
                   (GetFace(Sps::UBR_R) == Fce::R || GetFace(Sps::UBR_R) == Fce::L) &&
                   (GetFace(Sps::DFR_R) == Fce::R || GetFace(Sps::DFR_R) == Fce::L) &&
                   (GetFace(Sps::DBR_R) == Fce::R || GetFace(Sps::DBR_R) == Fce::L);
        case Lyr::L: 
            return (GetFace(Sps::UL_L) == Fce::L || GetFace(Sps::UL_L) == Fce::R) &&
                   (GetFace(Sps::DL_L) == Fce::L || GetFace(Sps::DL_L) == Fce::R) &&
                   (GetFace(Sps::FL_L) == Fce::L || GetFace(Sps::FL_L) == Fce::R) &&
                   (GetFace(Sps::BL_L) == Fce::L || GetFace(Sps::BL_L) == Fce::R) &&
                   (GetFace(Sps::UFL_L) == Fce::L || GetFace(Sps::UFL_L) == Fce::R) &&
                   (GetFace(Sps::UBL_L) == Fce::L || GetFace(Sps::UBL_L) == Fce::R) &&
                   (GetFace(Sps::DFL_L) == Fce::L || GetFace(Sps::DFL_L) == Fce::R) &&
                   (GetFace(Sps::DBL_L) == Fce::L || GetFace(Sps::DBL_L) == Fce::R);
        default: return false;
        }
    }

    // Get given pieces report for current cube
    std::string Cube::GetReport(const Pgr PG) const
    {
        std::string Report;
        std::vector<Pcp> PosList; // Positions for report
        Cube::AddToPositionsList(PG, PosList);
        for (const auto Pos : PosList)
        {
            Report += "Piece in " + GetPiecePositionText(Pos) + " position: " + GetPieceText(GetPiece(Pos));
            Report += " with direction " + GetPieceDirectionText(GetPieceDirection(Pos));
						Report += CheckOrientation(Pos) ? " (orientation OK)\n" : " (orientation NOT OK)\n";
        }
        return Report;
    }
}
