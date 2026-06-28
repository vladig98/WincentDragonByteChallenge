#include <bits/stdc++.h>

using namespace std;

struct vert {
    char c;
    int comp = -1;
};

struct component {
    // minimum and maximum coordinate values
    int lo[2], hi[2];
    // whether this # component has a hole inside
    bool has_hole = false;
};

int HORIZONTAL_COORD;
vector<vector<vert> > g;
vector<component> components;
int dx[] = {-1,0,1,0};
int dy[] = {0,1,0,-1};
int r,c;

bool order_horizontally(component a,component b) {
    return a.lo[HORIZONTAL_COORD] < b.lo[HORIZONTAL_COORD];
}

bool overlap(int i,int k,int idx) {
    pair<int,int> p[4] = { {components[i].lo[idx],0}, {components[i].hi[idx],0}, {components[k].lo[idx],1}, {components[k].hi[idx],1} };
    sort(p,p+4);
    return (p[0].second != p[1].second);
}

// flood the component at [x][y], number it comp
// if comp >= 0, we are flooding a letter, so compute its minimum/maximum coordinates
// and check if it has a hole
void bfs(int x,int y,char chr,int comp) {
    queue<int> q;
    q.push(x);
    q.push(y);
    g[x][y].comp = comp;

    if(comp >= 0) {
        components.resize(comp+1);
        components.back().lo[0] =  components.back().hi[0] = x;
        components.back().lo[1] = components.back().hi[1] =  y;
    }

    while(!q.empty()) {
        x = q.front(); q.pop();
        y = q.front(); q.pop();

        if(comp >= 0) {
            components[comp].lo[0] = min(components[comp].lo[0],x);
            components[comp].hi[0] = max(components[comp].hi[0],x);
            components[comp].lo[1] = min(components[comp].lo[1],y);
            components[comp].hi[1] = max(components[comp].hi[1],y);
        }

        for(int d=0;d<4;++d) {
            int nx = x + dx[d];
            int ny = y + dy[d];

            if(nx<0||nx==r||ny<0||ny==c) continue;

            // if it's a letter, check for hole
            if(comp >= 0 && g[nx][ny].comp == -1 && g[nx][ny].c != chr) {
                components[comp].has_hole = true;
            }

            if(g[nx][ny].c != chr || g[nx][ny].comp != -1) continue;
            
            g[nx][ny].comp = comp;
            q.push(nx); 
            q.push(ny);
        }
    }
}

int solve()
{
    cin >> r >> c;
    g.clear();
    g.resize(r,vector<vert>(c));
    components.clear();

    for(int i=0;i<r;++i)
    for(int k=0;k<c;++k)
        cin >> g[i][k].c;

    // eat up outside 
    bfs(0,0,'.',-2);


    for(int i=0;i<r;++i)
    for(int k=0;k<c;++k)
        if(g[i][k].c == '#' && g[i][k].comp == -1)
            bfs(i,k,'#',components.size());

    int holes = 0;
    for(int i=0;i<components.size();++i)
        if(components[i].has_hole) holes++;

    int comps = components.size();

    // zero = 4,2
    if(comps==4 && holes==2) return 0;
    // one = 3 2
    if(comps==3 && holes==2) return 1;
    // two = 3 1
    if(comps==3 && holes==1) return 2;
    // four = 4 1
    if(comps==4 && holes==1) return 4;
    // six = 4 0
    if(comps==4 && holes==0) return 6;
    // eight = 6 2
    if(comps==6 && holes==2) return 8;

    // five or nine, three or seven
    // need to know word orientation
    HORIZONTAL_COORD = 0;
    int overlaps = 0;
    for(int i=0;i+1<components.size();++i)
    for(int k=i+1;k<components.size();++k)
        overlaps += overlap(i,k,0);
    

    // i overlaps with itself, if there's more then it's vertically oriented
    if(overlaps > 1) HORIZONTAL_COORD = 1;

    sort(components.begin(),components.end(),order_horizontally);

    // three = 5 2, seven = 5 2
    if (comps==5 && holes==2) {
        // if hole in second and fourth letter, then seven, otherwise three
        if(components[1].has_hole && components[3].has_hole) return 7;
        return 3;
    }

    // five = 5 1, nine = 5 1
    if(comps==5 && holes==1) {
        // dot on i in nine does not overlap horizontally with anything
        // in five it overlaps with f
        int VERT = (HORIZONTAL_COORD+1)%2;
        int i=1, I=2, f = 0;
        if(components[0].has_hole) i=2, I=3, f=4;

        // we can just check both components of i, since the 'leg' overlaps everytime
        if(overlap(i,f,VERT) && overlap(I,f,VERT)) return 5;
        return 9;
    }

    // shouldn't happen
    cerr << "cannot decide on " << comps << " " << holes << "\n";
    return -1;
}

int main()
{
    int t;
    cin >> t;
    while(t--)
        cout << solve() << "\n";
    
}