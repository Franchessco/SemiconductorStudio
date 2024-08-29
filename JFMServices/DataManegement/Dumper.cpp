#include "Dumper.hpp"
#include "IDataManager.hpp"
#include <yaml-cpp/yaml.h>

namespace JFMService
{
    std::string transferModelIDToString(ModelID id)
    {
        switch (id)
        {
        case JFMModelID::Model4P:
            return "FourParameterModel";
        case JFMModelID::Model6P:
            return "SixParameterModel";
        default:
            return "";
        }
    };

    void serializeParameters(YAML::Emitter &emitter, const ParameterMap &parameters)
    {
        using namespace DataManagementService;
        std::vector<ParameterName> order{"A", "I0", "Rs", "Rsh", "alpha", "Rsh2"}; // NOTE this should be static and defined in the API of module!!
        for (const auto &name : order)
        {
            try
            {
                emitter << parameters.at(parameterNameToID(name));
            }
            catch (const std::exception &)
            {
                continue;
            }
        }
    };
    void MonteCarloResultDumper::Save(const path &path, const LoaderOutput &toSave, const Callback &callback)
    {
        MCOutput data = *(toSave).mcData;
        YAML::Emitter out;

        emitConfig(out, data.inputData);
        emitMCSimulation(out, data.mcResult);
        std::ofstream output(path);
        output << out.c_str();
    };

    void MonteCarloResultDumper::Save(const std::vector<path> &path, const std::vector<LoaderOutput> &toSave, const VectorCallback &callback)
    {
    }
    void MonteCarloResultDumper::emitConfig(YAML::Emitter &emitter, const MCInput &toEmit)
    {
        emitter << YAML::BeginMap;

        emitter << YAML::Key << "model" << YAML::Value << transferModelIDToString(toEmit.startingData.initialData.modelID);
        emitter << YAML::Key << "relativePath" << YAML::Value << toEmit.relPath.c_str();
        emitter << YAML::Key << "CharacteristicName" << YAML::Value << std::string(toEmit.startingData.name);
        emitter << YAML::Key << "idealParameters" << YAML::Value << YAML::BeginSeq;
        serializeParameters(emitter, toEmit.trueParameters);
        emitter << YAML::EndSeq;
        emitter << YAML::Key << "Temperature" << YAML::Value << toEmit.startingData.initialData.additionalParameters.at(Fitters::AdditionalParametersID::Temperature);
        emitter << YAML::Key << "numberOfIterations" << YAML::Value << toEmit.iterations;
        emitter << YAML::Key << "fixingConfiguration" << YAML::BeginSeq;
        for (const auto [id, val] : toEmit.startingData.fixConfig)
            emitter << parameterIdToString((Fitters::ParameterID)id);
        emitter << YAML::EndSeq;
        emitter << YAML::Key << "fixingValues" << YAML::BeginSeq;
        for (const auto [id, val] : toEmit.startingData.fixConfig)
            emitter << val;
        emitter << YAML::EndSeq;
        emitter << YAML::Key << "order" << YAML::BeginSeq;
        for (const auto item : {"A", "I0", "Rs", "Rsh", "alpha", "Rsh2"})
        {
            auto it = toEmit.startingData.fixConfig.find(DataManagementService::parameterNameToID(item));
            if (it != toEmit.startingData.fixConfig.end())
                emitter << item;
        }
        emitter << YAML::EndSeq;
        emitter << YAML::Key << "noise" << YAML::Value << toEmit.noise;
        emitter << YAML::EndMap;
    }
    void MonteCarloResultDumper::emitMCSimulation(YAML::Emitter &emitter, const std::vector<MCResult> &simulation)
    {
        // data
        emitter << YAML::BeginMap;
        emitter << YAML::Key << "data" << YAML::Value;
        emitter << YAML::BeginSeq;
        for (const auto &result : simulation)
        {
            // result
            emitter << YAML::BeginMap << YAML::Key << "result";
            emitter << YAML::Value << YAML::BeginSeq;
            serializeParameters(emitter, result.foundParameters);
            emitter << YAML::EndSeq;
            emitter << YAML::Key << "error" << YAML::Value << result.error << YAML::EndMap;
        }

        emitter << YAML::EndSeq; // parameters
        emitter << YAML::EndMap; // data
    };
}