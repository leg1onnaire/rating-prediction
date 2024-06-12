#include <iostream>
#include <unordered_map>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>


int main()
{
    int num_of_person = 0;
    int num_of_items = 0;

    int rating_num[35184][2];
    std::map<int, int> movie_ratings;
    for(int i = 1; i <= 8927; i++)
    {
        movie_ratings.insert(std::pair<int, int>(i, 0));
    }
    for(int i = 0; i < 35184; i++)
    {
        rating_num[i][0] = i;
        rating_num[i][1] = 0;
    }


    std::vector<float>* ptrvec;
    ptrvec = new std::vector<float>[1048575];//Veri sayisi buradan degistirilecek
    for (int i = 0; i < 1048575; i++)//Veri sayisi buradan degistirilecek
    {
        (*(ptrvec + i)).clear();
    }

    std::map<int, std::vector<float>*> mainmap1;

    std::fstream data;

    data.open("train.csv", std::ios::in);
    std::string holder, line, word;
    std::vector<float> row;
    float num = 0;
    int counter = 0;

    while (data >> holder && counter < 1048575)//Veri sayisi buradan degistirilecek
    {
        row.clear();
        std::stringstream s(holder);

        while (std::getline(s, word, ','))
        {
            num = std::stof(word);
            row.push_back(num);
        }

        *(ptrvec + counter) = row;
        mainmap1.insert(std::pair<int, std::vector<float>*>(counter + 1, (ptrvec + counter)));
        //		mainmap1[counter + 1] = (ptrvec + counter);
        if (row[0] >= num_of_person) { num_of_person = row[0]; }//35184-->en buyuk kullanici numarasini buluyor.
        if (row[1] >= num_of_items) { num_of_items = row[0]; }//8927-->en buyuk item numarasini buluyor.
        int vecrow = row[0];//kisiler
        int movrow = row[1];//itemler
        rating_num[vecrow][1] = rating_num[vecrow][1] + 1;
        movie_ratings[movrow] = movie_ratings[movrow] + 1;

        counter++;
    }

    //top10 itemleri tutacagimiz arrayler declare ediliyor ardindan 0 degeri ile initialize ediliyor.
    int top10[10][2];
    int top10_item[10][2];
    for(int i = 0; i < 10; i++)
    {
        top10[i][0] = 0;
        top10[i][1] = 0;
    }
    for(int i = 0; i < 10; i++)
    {
        top10_item[i][0] = 0;
        top10_item[i][1] = 0;
    }

    //Buyukten kucuge siralarken daha buyuk degerin altinda olma ve mevcut degerden daha buyuk olma
    //sartlari bulundugundan, once en buyuk ilk degeri buluyoruz.
    //Daha sonra diger 9 degeri buluyoruz.
    for(int i = 0; i < 35184; i++)
    {
        if(rating_num[i][1] > top10[0][1])
        {
            top10[0][0] = rating_num[i][0];
            top10[0][1] = rating_num[i][1];
        }
    }
    for (int i = 1; i <= 8927; i++)
    {
        if (movie_ratings[i] > top10_item[0][1])
        {
            top10_item[0][0] = i;
            top10_item[0][1] = movie_ratings[i];
        }
    }
    for(int i = 1; i < 10; i++)
    {
        for(int j = 0; j < 35184; j++)
        {
            if(rating_num[j][1] > top10[i][1] && rating_num[j][1] < top10[i-1][1])
            {
                top10[i][0] = rating_num[j][0];
                top10[i][1] = rating_num[j][1];
            }
        }
    }



    for(int i = 1; i < 10; i++)
    {
        for(int j = 1; j <= 8927; j++)
        {
            if(movie_ratings[j] > top10_item[i][1] && movie_ratings[j] < top10_item[i-1][1])
            {
                top10_item[i][0] = j;
                top10_item[i][1] = movie_ratings[j];
            }
        }
    }
    //yazdirma-----------------------
    std::cout << "UserID NumberOfRatings" << std::endl;
    for (int i = 0; i < 10; i++)
    {
        std::cout << top10[i][0] << "  " << top10[i][1] << std::endl;
    }
    std::cout << std::endl;
    std::cout << "MovieId  NumberOfRatings" << std::endl;
    for(int i = 0; i < 10; i++)
    {
        std::cout << top10_item[i][0] << "  " << top10_item[i][1] << std::endl;
    }

    delete[] ptrvec;

    return 0;
}