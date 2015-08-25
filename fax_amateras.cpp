#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define DEBUG 0
#if DEBUG
#   define LOG(...)    printf(__VA_ARGS__)
#else
#   define LOG(...)    ((void)0)
#endif

class Line {
public:
    struct Segment {
        int32_t x1, x2; // x2 - x1 is length
        int32_t gid;
        Segment *next;
    };

private:
    int32_t m_width;
    Segment *m_segments, *m_ptr, *m_prev;

    int32_t m_groups;
    int32_t m_gid;

public:
    Line(int32_t width);
    ~Line();
    void draw(bool is_same, int32_t& io_length);
    int32_t getGroups() { return m_groups; }
};

Line::Line(int32_t width) :
    m_width(width),
    m_segments(NULL), m_ptr(NULL), m_prev(NULL),
    m_groups(0), m_gid(0)
{
    /* nothing to do */
    m_segments = new Segment();
    m_segments->next = NULL;
    m_segments->gid = 0;
    m_segments->x1 = 0;
    m_segments->x2 = m_width;

    m_ptr = m_segments;
    m_prev = NULL;
}

Line::~Line()
{
    while (m_segments) {
        m_ptr = m_segments;
        m_segments = m_ptr->next;
        delete m_ptr;
    }
}

void
Line::draw(bool is_same, int32_t& io_length)
{
    int32_t x1 = m_ptr->x1;
    int32_t x2 = x1+io_length;
    if (x2>m_width) x2 = m_width;

    assert(x1!=x2);
    LOG("%s %d %d-%d\n", is_same?"S":"D", io_length, x1, x2);

    while (m_ptr && m_ptr->x1<x2) {
        if (x2<m_ptr->x2) {
            Segment *n = new Segment();
            n->next = m_ptr->next;
            m_ptr->next = n;
            n->x1 = x2;
            n->x2 = m_ptr->x2;
            m_ptr->x2 = x2;
            n->gid = m_ptr->gid;
        }
        if (!is_same) {
            if (m_ptr->gid>0) {
                m_ptr->gid = 0;
            } else {
                m_ptr->gid = ++m_gid;
                m_groups++;
                LOG("INC %d,%d\n", m_ptr->x1, m_ptr->x2);
            }
        }
        if (m_ptr->gid>0 && m_prev && m_prev->gid>0
                && m_prev->gid!=m_ptr->gid) {
            LOG("DEC %d,%d\n", m_ptr->x1, m_ptr->x2);
            int gid = m_ptr->gid;
            for (Segment *ptr = m_segments ; ptr ; ptr = ptr->next) {
                if (ptr->gid==gid)
                    ptr->gid = m_prev->gid;
            }
            m_groups--;
        }
        m_prev = m_ptr;
        m_ptr = m_ptr->next;
    }
    if (m_ptr==NULL) {
        LOG("NEW LINE %d\n", m_groups);
        for (m_ptr=m_segments ; m_ptr ; m_ptr = m_ptr->next) {
            LOG("[%d] %d-%d ", m_ptr->gid, m_ptr->x1, m_ptr->x2);
        }
        LOG("\n");
        m_ptr = m_segments;
        m_prev = NULL;
    }
    io_length -= x2-x1;
}

int
main(int argc, const char *argv[])
{
    int32_t width;
    while (scanf("%d", &width)>0) {
        if (width<0) break;

        int total_runs, runs;
        scanf("%d %d", &total_runs, &runs);

        Line line(width);
        bool is_same = true;
        int32_t length = 0;
        for (int i=0 ; i<total_runs ; i++) {
            scanf("%d", &length);
            while (length>0)
                line.draw(is_same, length);
            is_same = !is_same;
        }
        printf("%d\n", line.getGroups());
    }
    return 0;
}
