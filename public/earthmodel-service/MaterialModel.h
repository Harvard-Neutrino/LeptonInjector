#ifndef LI_MaterialModel_H
#define LI_MaterialModel_H

#include <map>
#include <string>
#include <vector>

namespace earthmodel {

class MaterialModel {
    static constexpr const int CHAR_BUF_SIZE = 8196;
private:
    std::string path_;
    std::vector<std::string> model_files_;

    std::vector<std::string> material_names_;
    std::map<std::string, int> material_ids_;
    std::map<int, std::map<int, double> > material_maps_;
    std::map<int, double> pne_ratios_;
public:
    MaterialModel();
    MaterialModel(std::string const & path);
    MaterialModel(std::string const & path, std::string const & matratio);
    MaterialModel(std::string const & path, std::vector<std::string> const & matratios);

    void SetPath(std::string const & path);
    void AddMaterial(std::string const & name, int matpdg, std::map<int, double> matratios);
    void AddMaterial(std::string const & name, double pne_ratio, std::map<int, double> matratios);
    void AddModelFiles(std::vector<std::string> const & matratios);
    void AddModelFile(std::string matratio);

    double GetPNERatio(int id);
    std::string GetMaterialName(int id);
    int GetMaterialId(std::string const & name);
    bool HasMaterial(std::string const & name);
    bool HasMaterial(int);
private:
    double ComputePNERatio(int id);
public:
    static void GetAZ(int code, int & np, int & nn);
};

} // namespace earthmodel

# endif // LI_MaterialModel_H
