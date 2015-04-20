#ifndef DBOUTPUT_H
#define DBOUTPUT_H

#include <QList>
#include <QTextStream>
#include <QFile>

class Action;
class Character;
class Game;

/**
 * @brief The DBOutput class provides an instance of database output for Game
 * objects.
 *
 * The initial purpose of the Hearthstone Simulator was to build and AI based
 * on machine learning techniques. The database of samples we used were created
 * using this class, along with the generation mode of the Game class.
 *
 * A DBOutput object generates two files based on a single Game. The files are
 * effectively written at the moment its attached game is finished. There is
 * one file for logging information relative to PlayAction's, and another one
 * about TargetedAction's.
 *
 * Both files samples feature the game environment at time <i>(t - 1)</i> as
 * their first features. Then,
 * \li PlayAction's file: a single feature is added - the numerical ID of the
 * card played.
 * \li TargetedAction's file: several features are added - features describing
 * the action and features describing the chosen target.
 * The output associated to each sample is the value of the Board Control value
 * function (see Game::meta_BoardControlScore()) based on environment at time
 * \e t.
 *
 * The files generated can be prefixed with a string of your choice, using the
 * SetFilenamesPrefix() static method.
 *
 * @ingroup hsengine
 * @sa Game::IsDBGenerationModeOn()
 */
class DBOutput
{
public:
    /**
     * @brief Gets the DBOutput instance bound to a Game.
     *
     * If none was already bound, this will create one.
     *
     * @param g The Game you want to get the DBOutput instance of.
     *
     * @return the DBOutput instance bound to Game \a g.
     */
    static DBOutput* Instance(Game* g);

    /**
     * @brief Flushes the DBOutput bound to a Game and deletes it.
     *
     * This will create two files in a "generated/" folder, itself created in
     * the working directory. The two filenames will be
     * <code>[prefix].play.csv</code> and <code>[prefix].target.csv</code>.
     *
     * If the files already exist, this will only append the buffered samples.
     *
     * In a multithreaded environment, in the case where multiples games are
     * run simultaneously, this will still flush to the same file for all
     * games. This is however done safely (only one thread at a time will be
     * able to flush its buffered data into the files).
     *
     * @param g The Game you want to flush the DBOutput of.
     */
    static void DestroyInstance(Game* g);

    /**
     * @brief Sets the generated filenames prefix.
     *
     * The two filenames will be <code>prefix.play.csv</code> and
     * <code>prefix.target.csv</code>.
     *
     * The default prefix is \e db.
     *
     * @param prefix The prefix you want to see in front of the generated files
     * names.
     */
    static void SetFilenamesPrefix(const QString& prefix);

    /**
     * @brief Creates a new sample and buffers an environment and a
     * TargetedAction for it.
     *
     * If another sample was being buffered, it is not lost; the new one is
     * put in another buffer line.
     *
     * @param environment The environment to buffer.
     * @param a The TargetedAction to buffer.
     */
    void buffer(const QVector<float>& environment, Action* a);

    /**
     * @brief Buffers the target of the TargetedAction which is in the current
     * buffer line.
     *
     * The features of \a target will be appended to the current buffer line.
     *
     * @warning This can be called only after a call to
     * buffer(const QVector<float>& environment, Action* a).
     *
     * @param target The target whose features should be buffered.
     * @param a The TargetedAction which targets \a target; it is the
     * TargetedAction in the current buffer line.
     */
    void buffer(Character* target, Action* a);

    /**
     * @brief Buffers the output of the sample which is in the current buffer
     * line.
     *
     * The output will be appended to the current buffer line.
     *
     * @warning This can be called only after a call to
     * buffer(Character* target, Action* a).
     *
     * @param score The output to buffer.
     * @param a The TargetedAction in the current buffer line.
     */
    void buffer(float score, Action *a);
    void addEntry(const QVector<float>& environment, Action* a, float score);

private:
    enum BufferEntryType {
        ENVIRONMENT,
        TARGET
    };

    static QString mFilenamesPrefix;
    static QHash<int, DBOutput*> mInstances;
    QString mPlayActionFile;
    QString mTargetedActionFile;
    QTextStream mOs_playAction;
    QTextStream mOs_targetedAction;
    QStringList mBuffer;
    BufferEntryType mLastInsertionInBuffer;

    DBOutput();
    ~DBOutput();
    void mInsertEnvironment(const QVector<float>& environment, QTextStream &os);
    void mInsertTarget(const Character* target, QTextStream &os);
};

#endif // DBOUTPUT_H
