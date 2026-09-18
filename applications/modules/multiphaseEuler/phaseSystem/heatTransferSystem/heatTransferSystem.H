/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2015-2026 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::heatTransferSystem

Description
    Class which provides interfacial momentum transfer between a number of
    phases. Drag, virtual mass, lift, wall lubrication and turbulent dispersion
    are all modelled. Access is provided to a various forces and coefficients
    needed by the solver.

SourceFiles
    heatTransferSystem.C

\*---------------------------------------------------------------------------*/

#ifndef heatTransferSystem_H
#define heatTransferSystem_H

#include <optional>
#include <exception>

#include "phaseSystem.H"

#include "fvmSup.H"

#include "heatTransferModel.H"

#include "twoResistanceHeatTransfer.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward declaration of classes
class blendedHeatTransferModel;
class blendedSidedHeatTransferModel;

/*---------------------------------------------------------------------------*\
                 Class heatTransferSystem Declaration
\*---------------------------------------------------------------------------*/

class heatTransferSystem
:
    public IOdictionary
{
    // Protected Type Definitions
protected:
    //- Table of one-resistance heat transfer models
        typedef
            HashPtrTable
            <
                blendedHeatTransferModel,
                phaseInterfaceKey,
                phaseInterfaceKey::hash
            > modelsTable;

        //- Table of two-resistance heat transfer models
        typedef
            HashPtrTable
            <
                blendedSidedHeatTransferModel,
                phaseInterfaceKey,
                phaseInterfaceKey::hash
            > sidedModelsTable;


    // Private Data

        //- Reference to the phase system
        const phaseSystem& fluid_;

        //- One-resistance heat transfer models
        modelsTable models_;

        //- Two-resistance heat transfer models
        sidedModelsTable sidedModels_;


    // Private Member Functions

        //- Create IO object for an optional constant/heatTransfer file
        IOobject io(const phaseSystem&) const;

        //- Return the models dictionary
        const dictionary& modelsDict() const;

        //- Read the models
        void readModels();

        //- Return the two-resistance heat transfer coefficients between a pair
        //  of phases with additional arguments
        template<class ... Args>
        Pair<tmp<volScalarField>> Hs
        (
            const phaseModel& phase1,
            const phaseModel& phase2,
            Args ... args
        ) const {
        auto error = [&](const word& why) {
            auto err_str = why +
                " two-resistance heat transfer models found that provide a heat transfer coefficient between phases " +
                phase1.name() + " and " + phase2.name();
            throw std::runtime_error{err_str};
        };

        autoPtr<Pair<tmp<volScalarField>>> HsPtr;

        const phaseInterface interface(phase1, phase2);

        auto iter = sidedModels_.find(interface);

        if (iter != sidedModels_.end()) {
            HsPtr.set
            (
                new Pair<tmp<volScalarField>>(
                    iter()->KinThe(phase1, args...),
                    iter()->KinThe(phase2, args...)
                )
            );
        }

        const Foam::fvModels& fvModels = Foam::fvModels::New(fluid_.mesh());

        forAll(fvModels, i) {
            if (!isA<fv::twoResistanceHeatTransfer>(fvModels[i])) continue;

            const fv::twoResistanceHeatTransfer& heatTransferFvModel =
                refCast<const fv::twoResistanceHeatTransfer>(fvModels[i]);

            Pair<tmp<volScalarField>> Hs =
                heatTransferFvModel.Ks(phase1, phase2, args...);

            if (!Hs.first().valid() || !Hs.second().valid()) continue;

            if (HsPtr.valid()) error("Multiple");

            HsPtr.set(new Pair<tmp<volScalarField>>(Hs.first(), Hs.second()));
        }

        if (!HsPtr.valid()) error("No");

        return HsPtr();
    }

public:

    //- Runtime type information
    TypeName("heatTransferSystem");


    //- Default name of the phase properties dictionary
    static const word propertiesName;


    // Constructors

        //- Construct from a phase system
        heatTransferSystem(const phaseSystem&);


    //- Destructor
    virtual ~heatTransferSystem();


    // Member Functions

        //- Return the two-resistance heat transfer coefficients between a pair
        //  of phases
        Pair<tmp<volScalarField>> Hs
        (
            const phaseModel& phase1,
            const phaseModel& phase2
        ) const;

        //- Return the two-resistance heat transfer coefficients between a pair
        //  of phases with a specified residual volume fraction
        Pair<tmp<volScalarField>> Hs
        (
            const phaseModel& phase1,
            const phaseModel& phase2,
            const scalar residualAlpha
        ) const;

        //- Return effective heat transfer coefficients between a pair of
        // phases. Due to stabilization tricks, swappiness is not strictly gaurentted
        std::optional<volScalarField::Internal> HEff(
            const phaseModel& from,
            const phaseModel& to
        ) const;

    //- Return the heat transfer matrices
    autoPtr<HashPtrTable<fvScalarMatrix>> heatTransfer() const;

    //- Read base phaseProperties dictionary
        virtual bool read();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
