#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <iostream>
#include <unordered_map>
#include <fstream>

using namespace std;

extern char **environ;

namespace Envir {

    class Environment {
        private:
            string path;
            unordered_map<string, string> envs;

            char *value;

        public:
            Environment(string path){
                this->path = path;

                for(int i = 0; environ[i] != NULL; i++){
                    string env = environ[i];
                    size_t pos = env.find("=");
                    string key = env.substr(0, pos);
                    string value = env.substr(pos + 1);

                    envs[key] = value;
                }

                ifstream file(path);

                if(file.is_open()) {
                    string line;
                    while(getline(file,line)){
                        auto pos = line.find("=");
                        string key = line.substr(0, pos);
                        string value = line.substr(pos + 1);

                        envs[key] = value;
                    }
                }else{
                    cout << "ENV file not found" << endl;
                    return;
                }

                file.close();
            }

            pair<string, string> get_env(string key){
                return make_pair(key, envs[key]);
            }
    };
};

#endif // ENVIRONMENT_H