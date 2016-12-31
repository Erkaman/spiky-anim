#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include <vector>
#include <string>

using std::vector;
using std::string;
using std::to_string;

#define PI 3.14

const int WIDTH = 512;
const int HEIGHT = 512;

std::string zeroPad(int n, int len) {
    string str = "";

    str = to_string(n);

    while(str.size() < len) {
        str = "0" + str;
    }

    return str;
}

class vec2 {
public:
    float x;
    float y;

    vec2(float x, float y) {
        this->x = x;
        this->y = y;
    }

    vec2() {
        this->x = this->y = 0;
    }

};

void makeSpiky(FILE* fp, float rad, string color) {
    int N = 24;

    // spiky center in image center.
    float PX = WIDTH/2;
    float PY = HEIGHT/2;

    vector<vec2> pos(N);

    float SMALLR = rad * 0.20f;
    float R1 = rad - SMALLR;
    float R2 = rad + SMALLR;

    // generate points on circle. But displace the points in direction of normal.
    // so that it becomes spiky.
    for(int i = 0; i < N; i++) {
        float theta = (i / (float)N) * 2.0 * PI;

        float R = i % 2 == 0 ? R1 : R2;

        pos[i].x = PX + R * cos(theta);
        pos[i].y = PY + R * sin(theta);
    }

    string str;

    // now make line paths between the generated points.
    for(int i = 0; i < (N+1); i++) {
        vec2 v = pos[(i) % N];

        if(i == 0) {
            // for first point, we just position the cursor.
            str += "<path d=\"M" + to_string(v.x) + ", " + to_string(v.y);
        } else {
            str += "L " +     to_string(v.x) + " " + to_string(v.y) + " ";
        }
    }

    str += "Z\n"; // close loop. path is now done.
    str += "\" />";

    float strokeWidth = 2.0f + 10.0f * (rad / 100.0f);
    if(strokeWidth > 8.0f) { // clamp stroke-width to max 8.0f
        strokeWidth = 8.0f;
    }

    fprintf(fp,"<g fill=\"none\" stroke=\"%s\"  stroke-width=\"%f\" fill-opacity=\"1.0\">", color.c_str(), strokeWidth);
    fprintf(fp,"%s", str.c_str() );

    fprintf(fp,"</g>");

}

struct Star {
    float rad;
    string color;

    Star(float rad, string color) {
        this->rad = rad;
        this->color = color;
    }
};

int main(int argc, char** argv) {
    // create output dir.
    system("mkdir -p temp/");

    float START_RAD = 3.0f;
    float SPAWN_RAD = 40.0f;

    // colors of spikys.
    const int NC = 3;
    string colors[NC] = {
        "#990022",
        "#888822",
        "#9C8066",
    };

    int ic = 0;
    string curColor = colors[ic];

    vector<Star> stars;

    stars.push_back(Star(START_RAD, curColor));

    for(int i = 0; i < 300; i++) { // 1000

        FILE* fp = fopen(("temp/out" + zeroPad(i, 5) + ".svg").c_str(), "w");

        fprintf(fp, "<svg width=\"%f\" height=\"%f\" version=\"1.1\" baseProfile=\"full\" xmlns=\"http://www.w3.org/2000/svg\">\n", (float)WIDTH, (float)HEIGHT);

        // clear to black background.
        fprintf(fp,"<rect ");
        fprintf(fp,"width=\"%d\" ", WIDTH);
        fprintf(fp,"height=\"%d\" ", HEIGHT);
        fprintf(fp,"x=\"0\" ");
        fprintf(fp,"y=\"0\" ");
        fprintf(fp,"fill=\"#000000\"/>\n");

        for(int j = 0; j < stars.size(); j++) {
            Star& star = stars[j];

            makeSpiky(fp, star.rad, star.color);
            star.rad+=2.0f; // spiky velocity.
        }

        // once last spiky is big enough, spawn another one.
        const float& lastRad = stars[stars.size()-1].rad;
        if(lastRad > SPAWN_RAD) {
            ic = (ic+1)%NC;
            curColor = colors[ic];

            stars.push_back(Star(START_RAD, curColor));
        }

        fprintf(fp, "</svg>");
        fclose(fp);
    }
    printf("create svgs\n");

    system("cd temp/ && for i in *.svg; do rsvg-convert $i -o `echo $i | sed -e 's/svg$/png/'`; done");
    printf("converted svgs to png\n");

    // frames from (225) to (225+58) makes a good loop.
    system("cd temp/ && ffmpeg -start_number 225 -y -framerate 30 -i  out%05d.png -vframes 58 -c:v libx264 -vf \"scale=512:512\" -r 30 ../out.mp4 ");
    printf("encoded to mp4\n");


    return 0;
}
