#include <iostream>
#include <vector>
#include <string>
#include <string>
#include <tuple>
#include <algorithm>
#include <sstream>

class HypothesisBuffer {
    public:
        std::vector<std::tuple<double, double, std::string>> self_committed_in_buffer;
        std::vector<std::tuple<double, double, std::string>> self_buffer;
        std::vector<std::tuple<double, double, std::string>> self_new;
        double self_last_committed_time;
        std::string self_last_committed_word;

        // constructor
        HypothesisBuffer() {
            self_committed_in_buffer = std::vector<std::tuple<double, double, std::string>>();
            self_buffer = std::vector<std::tuple<double, double, std::string>>();
            self_new = std::vector<std::tuple<double, double, std::string>>();
            self_last_committed_time = 0;
            self_last_committed_word = "";
        };

        void print_info() {
            std::cout << "------ hypothesis buffer -----" << "\n";
            std::cout << "------ commmitted in buffer -----" << "\n";
            print_helper(self_committed_in_buffer);
            std::cout << "------ buffer -----" << "\n";
            print_helper(self_buffer);
            std::cout << "------ new -----" << "\n";
            print_helper(self_new);
        }

        void print_helper(std::vector<std::tuple<double, double, std::string>>& v) {
             for (size_t i=0; i<v.size(); i++) {
                std::cout << std::get<0>(v[i]) << " "
                          << std::get<1>(v[i]) << " "
                          << std::get<2>(v[i]) << " "
                          << "\n";
            }
        }

        void insert(std::vector<std::tuple<double, double, std::string>>& new_, double offset) {
            std::cout << "----- buffer.insert begin -----" << std::endl;
            // add the offset to the new word list
            std::transform(new_.begin(), new_.end(), new_.begin(),
                [offset](const std::tuple<double, double, std::string>& t) {
                    return std::make_tuple(std::get<0>(t)+offset, std::get<1>(t)+offset, std::get<2>(t));
                }
            );

            // update this->new_ if the word timestamp is in or after the last 0.1s of the last committed time
            std::copy_if(new_.begin(), new_.end(), std::back_inserter(self_new),
                [this](const std::tuple<double, double, std::string>& t) {
                    return std::get<0>(t) > self_last_committed_time - 0.1;
                }
            );

            // the loop
            if (!self_new.empty()) {
                double a, b;
                std::string t;
                std::tie(a, b, t) = self_new[0];
                if (std::abs(a - self_last_committed_time) < 1) {
                    if (!self_committed_in_buffer.empty()) {
                        size_t cn = self_committed_in_buffer.size();
                        size_t nn = self_new.size();

                        for (int i=1; i<=std::min((int)std::min(cn, nn), 5); i++) {
                            // get c
                            std::vector<std::string> extracted;
                            for (size_t j=i; j>=1; j--) {
                                extracted.push_back(
                                    std::get<2>(self_committed_in_buffer[self_committed_in_buffer.size()-j])
                                );
                            }

                            std::ostringstream oss;
                            for (size_t j=0; j<extracted.size(); j++) {
                                if (j!=0) {
                                    oss << " ";
                                }
                                oss << extracted[j];
                            }

                            std::string c = oss.str();

                            // get tail
                            extracted.clear();
                            oss.clear();
                            for (size_t j=1; j<=i; j++) {
                                if (j!=1) {
                                    oss << " ";
                                }
                                oss << std::get<2>(self_new[j-1]);
                            }

                            std::string tail = oss.str();

                            // compare and remove
                            if (c == tail) {
                                std::vector<std::string> words = {};
                                for (size_t j=0; j<i; j++) {
                                    std::stringstream ss;
                                    std::tie(a, b, t) = self_new[0];
                                    self_new.erase(self_new.begin());
                                    ss << a << " " << b << " " << t;
                                    words.push_back(ss.str());
                                }

                                oss.clear();
                                for (size_t j=0; j<=words.size(); j++) {
                                    if (j!=0) {
                                        oss << " ";
                                    }
                                    oss << words[j];
                                }
                                break;
                            }
                        }
                    }
                }

            }
            std::cout << "----- buffer.insert end -----" << std::endl;
        }

        std::vector<std::tuple<double, double, std::string>> flush() {
            std::cout << "----- buffer.flush begin -----" << std::endl;
            std::vector<std::tuple<double, double, std::string>> commit = {};
            double na, nb;
            std::string nt;
            while (!self_new.empty()) {
                std::tie(na, nb, nt) = self_new[0];
                if (self_buffer.empty()) {
                    break;
                }

                if (nt == std::get<2>(self_buffer[0])) {
                    commit.emplace_back(na, nb, nt);
                    self_last_committed_word = nt;
                    self_last_committed_time = nb;
                    self_buffer.erase(self_buffer.begin());
                    self_new.erase(self_new.begin());
                } else {
                    break;
                }
            }

            self_buffer = self_new;
            self_new.clear();
            self_committed_in_buffer.insert(self_committed_in_buffer.end(), commit.begin(), commit.end());
            std::cout << "----- buffer.flush end -----" << std::endl;
            return commit;
        }

        void pop_committed(double t) {
            std::cout << "----- buffer.pre_committed begin -----" << std::endl;
            while (!self_committed_in_buffer.empty() &&
                std::get<1>(self_committed_in_buffer[0]) <= t) {
                self_committed_in_buffer.erase(self_committed_in_buffer.begin());
            }
            std::cout << "----- buffer.pre_committed end -----" << std::endl;
        }

        std::vector<std::tuple<double, double, std::string>> complete() {
            std::cout << "----- buffer.complete begin and end -----" << std::endl;
            return self_buffer;
        }

};
