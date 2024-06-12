#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <unordered_map>
#include <algorithm>
#include <sstream>


using namespace std;
unordered_map<int, unordered_map<int, float>> mainData;
unordered_map<int, unordered_map<int, float>> mov_similarity;
vector<int>targetUser_id;
vector<int>targetUser_movies_id;


void readTrain(const string& csv_address) { //train dosyasını işleme sok

    fstream readFile;

    readFile.open(csv_address, ios::in);

    string user_id;
    string movie_id;
    string ratings;

    int user;
    int movie;
    float movieRatings;

    while (getline(readFile, user_id, ','), getline(readFile, movie_id, ','), getline(readFile, ratings)) {
        //while loop'un içinde getline kullanarak dosyadaki değerleri çekip gerekli değişken formatlarına dönüştürüyoruz.
        user = stoi(user_id);
        movie = stoi(movie_id);
        movieRatings = stof(ratings);

        mainData[user][movie] = movieRatings;//kullanıcı,film ve değerlendirmeleri haritanın içine yerleştir.
        /*mainData[int_user].emplace_back(int_movie, movieRatings);
          */
    }


    readFile.close();
}

void readTest(const string& test_path) {
    std::fstream testFile;
    testFile.open(test_path, std::ios::in);
    if (!testFile.is_open()) {
        std::cerr << "can't reach the test file" << std::endl;
    }
    std::string id;
    std::string testUser_id;
    std::string testMovie_id;
    int test_int_user;
    int test_int_movie;
    while (getline(testFile, id, ','), getline(testFile, testUser_id, ','), getline(testFile, testMovie_id)) {
        test_int_user = stoi(testUser_id);
        test_int_movie = stoi(testMovie_id);
        targetUser_id.push_back(test_int_user);
        targetUser_movies_id.push_back(test_int_movie);
    }
}


float predict(int target_user , int target_movie) { //aranan filme yakın kullanıcıları bularak tahmin oluştur.
// tahmini bulmak için ağırlıklı toplam formülünden yararlandık.

    float nominator = 0, denominator = 0;//formulu bulmak için pay ve payda değerleri oluşturup bunların değerleri için for döngüsü oluşturduk.
    /*for (const auto &[movie_id, similarity] : mov_similarity[target_user]) {
        if (similarity >= 0.6) {
            auto it = std::find_if(mainData[movie_id].begin(), mainData[movie_id].end(), [&](const auto &pair) {
                return pair.first == target_user;
            });
            nominator += similarity * it->second;
            denominator += similarity;
        }
    }
     */
    for (auto itr = mov_similarity[target_user].begin(); itr != mov_similarity[target_user].end(); itr++) {
        if (mainData[itr->first][target_movie] >= 0.6) {//eğer oran 0.6 ve 0.6'dan büyükse işleme sok.
            nominator += itr->second * mainData[itr->first][target_movie];
            denominator += itr->second;
        }

    }

    if (denominator == 0) {
        return 0;
    }


    return nominator / denominator;
}


float cosineSimilarity(unordered_map<int, float>& map1, unordered_map<int, float>& map2) { //cosine benzerliğini hesapla.
 /*float cosineSimilarity(const vector<pair<int,float>>& v1, const vector<pair<int, float>>& v2)*/

    // Bunun için mapten değerleri alıp gerekli aritmatik işleme sokup çıktı alıyoruz.

    int key1 = map1.begin()->first;
    int key2 = map2.begin()->first;
    if (mov_similarity[key1].find(key2) != mov_similarity[key1].end() || mov_similarity[key2].find(key1) != mov_similarity[key2].end()) {
        return mov_similarity[key1][key2];
    }

    float dotProduct = 0, sumSquares1 = 0, sumSquares2 = 0;
    for (auto& iter1 : map1) {
        auto iter2 = map2.find(iter1.first);
        if (iter2 == map2.end()) {
            continue;
        }
        dotProduct += iter1.second * iter2->second;
        sumSquares1 += iter1.second * iter1.second;
        sumSquares2 += iter2->second * iter2->second;
    }
    if (dotProduct == 0 || sumSquares1 == 0 || sumSquares2 == 0) {
        return 0.0;
    }
    return dotProduct / (sqrt(sumSquares1) * sqrt(sumSquares2));
}


int main() {

    std::fstream outFile;

    outFile.open(R"(C:\Users\KAAN\Desktop\output_predict21.txt)", std::ios::out);
    readTrain(R"(C:\Users\KAAN\Desktop\train.csv)");
    readTest(R"(C:\Users\KAAN\Desktop\test.csv)");

    int count = 0;
    auto target_itr = targetUser_movies_id.begin();

    for (int value : targetUser_id) {
        // Iterate over all users in the dataSet unordered map except the target user
        int value2 = *target_itr;
        for (auto current_user_it = mainData.begin(); current_user_it != mainData.end(); current_user_it++) {
            if (current_user_it->first == value) {
                continue;
            }
            /*
            auto it = std::find_if((*current_user_it).second.begin(), (*current_user_it).second.end(), [value2](const auto& p) { return p.first == value2; });
            if(it != (*current_user_it).second.end()) {
                float sim = cosineSimilarity(current_user_it->second, mainData[value]);
                //mov_similarity[(*current_user_it).first] = vector<pair<int, float>(value, sim)>;
                mov_similarity[current_user_it->first].push_back(make_pair(value, sim));
                mov_similarity[value].push_back(make_pair(current_user_it->first, sim));
            }
             */

            // Calculate the similarity between the current user that watched the target movie and the target user
            if ((((*current_user_it).second)).find(value2) != (*current_user_it).second.end()) {
                float sim = cosineSimilarity(current_user_it->second, mainData[value]);

                mov_similarity[current_user_it->first][value] = sim;
                mov_similarity[value][current_user_it->first] = sim;
            }

        }
        target_itr++;
        cout << " Number of users ID'S : "<< count << endl;
        count++;
    }

    cout << "All users counted" << endl;

    auto targetMovie_itr = targetUser_movies_id.begin();
    int i = 0;
    /*for (int valueFirst : targetUser_id) {
        float prediction = predict(valueFirst);
        outFile << i++ << "," << prediction << endl;
        if (targetMovie_itr != targetUser_movies_id.end()) {
            targetMovie_itr++;
        }
    }
     */

    for (int valueFirst : targetUser_id) {
        int valueSecond=*targetMovie_itr;
        float prediction = predict(valueFirst,valueSecond);
        outFile << i++ << "," << prediction << endl;
        if (targetMovie_itr != targetUser_movies_id.end()) {
            targetMovie_itr++;
        }
    }

    return 0;
}