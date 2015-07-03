#ifndef __ZGEN_SHIFTREDUCE_TYPES_ACTION_UTILS_H__
#define __ZGEN_SHIFTREDUCE_TYPES_ACTION_UTILS_H__

#include "shiftreduce/types/action.h"
#include "shiftreduce/types/internal_types.h"

namespace ZGen {

namespace ShiftReduce {

class ActionUtils {
public:
  /**
   * Judge if the given action is SHIFT
   *
   *  @param[in]  act   The action
   *  @return     bool  If the action is SHIFT, return true;
   *                    otherwise false.
   */
  static bool is_shift(const action_t& act);

  /**
   * Judge if the given action is LEFT-ARC
   *
   *  @param[in]  act   The action
   *  @return     bool  If the action is LEFT-ARC, return true;
   *                    otherwise false.
   */
  static bool is_left_arc(const action_t & act);

  /**
   * Judge if the given action is LEFT-ARC
   *
   *  @param[in]  act   The action
   *  @return     bool  If the action is LEFT-ARC, return true;
   *                    otherwise false.
   */
  static bool is_right_arc(const action_t & act);

  /**
   * Get gold action sequence.
   *
   *  @param[in]  reference The reference dependency instance.
   *  @param[out] actions   The oracle transition sequence.
   *  @return     int       The number of actions in transition
   */
  static int get_correct_actions(const dependency_t& reference,
      bool labeled,
      action_sequence_t& actions);

private:
  /**
   * Perform in order left to root, root to right deep first search to get
   * gold transition action sequence.
   *
   *  @param[in]  root      The currently visited node in tree.
   *  @param[in]  reference The reference dependency instance.
   *  @param[in]  tree      The tree.
   *  @param[out] actions   The oracle transition sequence.
   */
  static void get_correct_actions_travel(int root,
      bool labeled,
      const dependency_t& reference,
      const std::vector<std::vector<int> > & tree,
      action_sequence_t& actions);
};

} //  end for namespace ShiftReduce
} //  end for namespace ZGen

#endif  //  end for __ZGEN_SHIFTREDUCE_TYPES_ACTION_UTILS_H__
