#ifndef SELECTION_H
#define SELECTION_H

class Selection
{
public:
    enum State {
        EMPTY,
        DRAGGING,
        DONE
    };

    Selection() : mState(EMPTY), mStart(0), mEnd(0) {}

    inline void setStart(double start) {
        mStart = start;
        mState = DRAGGING;
    }
    inline void setEnd(double end) {
        mEnd = end;
        mState = DRAGGING;
    }
    inline void setDone() { mState = DONE; }
    inline void reset() { mState = EMPTY; }

    inline State state() const { return mState; }
    inline double start() const { return mStart; }
    inline double end() const { return mEnd; }
    inline double left() const { return std::min(mStart, mEnd); }
    inline double right() const { return std::max(mStart, mEnd); }
    inline double length() const { return right() - left(); }

private:
    State mState;
    double mStart;
    double mEnd;
};

#endif // SELECTION_H
