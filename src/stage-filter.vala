/*
 A simple filter based on a specified boolean column in the data matrix.
 For custom filters, insert a stage before this one that modifies
 the filter column.
*/
using GLib;

public class GGobi.StageFilter : Stage {
  public int filter_col { get; construct set; }

  /* filtered index ==> raw index */
  public uint[] included_rows;
  /* raw index ==> filtered index, -1 = filtered */
  public int[] included_rows_rev;
  /* whether a (raw) row passes the filter */
  private bool[] included;

  override void process_incoming(PipelineMessage msg) {
    SList<uint> changed_cols = msg.get_changed_cols();
    uint rows_changed = msg.get_n_added_rows() + 
      msg.get_n_removed_rows();
    msg.consume_rows(); // we ignore any row changes 
    // FIXME: need to check whether filter column has been shifted or removed
    /* rows added, rows removed, or filter column changed: update filter */
    if ((rows_changed > 0) || changed_cols.find(filter_col) != null)
      process_filter();
    base.process_incoming(msg);
  }

  construct {
    filter_col = -1;
  }

  // Returns: %TRUE if the parent row is allowed past the filter
  public bool is_included(uint i) {
    return included[i];
  }
  
  // This is a convenience function that sets the cell at row @i in the
  // filter column to @val in the parent stage.
  public void set_included(uint i, bool val) {
    parent.set_raw_value(i, filter_col, (double) val);
  }

  // Convenience function for setting the entire filter column at once.
  public void set_included_all(bool value) {
    for (uint i = 0; i < parent.n_rows; i++)
      set_included(i, value);
  }

  override void set_missing(uint i, uint j) {
    parent.set_missing(included_rows[i], j);
  }

  override bool is_missing(uint i, uint j) {
    return parent.is_missing(included_rows[i], j);  
  }

  public override string get_row_id(uint i) {
    return parent.get_row_id(included_rows[i]);  
  }

  override int get_row_for_id(string id) {
    int row = parent.get_row_for_id(id);
    if (row == -1)
      return -1;
    return included_rows_rev[row];
  }

  override void set_raw_value(uint i, uint j, double value) {
    parent.set_raw_value(included_rows[i], j, value);
  }

  override double get_raw_value(uint i, uint j) {
    return parent.get_raw_value(included_rows[i], j);
  }

  // Convenience method that refreshes the filter and
  // and flushes the queued changes.
  public void update() {
    process_filter();
    flush_changes_here();
  }

  /**
   * process_filter:
   * @self: a #GGobiStageFilter
   *
   * Update the included state of the rows, based on the result of 
   * included(). Most clients will just use
   * update() which is a higher-level function.
   *
   */
  public void process_filter() {
    int parent_nrows = parent != null ? (int) parent.n_rows : 0;
    SList<uint> removed_rows = new SList<uint>();
    int n_included = 0, n_added = 0;
    bool[] included_prev = new bool[parent_nrows];

    included_rows.resize(parent_nrows);
    included_rows_rev.resize(parent_nrows);
    included.resize(parent_nrows);

    for (int i = 0; i < parent_nrows; i++) {
      included_prev[i] = included[i];
      included[i] = (filter_col == -1) ? true : 
        (bool) parent.get_raw_value(i, filter_col);
    }
  
    for (int i = 0; i < parent_nrows; i++) {
      if (included[i]) {
        included_rows_rev[i] = n_included;
        included_rows[n_included++] = i;        
        
        // Included now, but wasn't before
        if (!included_prev[i]) n_added++;
      } else {
        // Not included now, but was before
        if (included_prev[i]) removed_rows.prepend(i);
      }
    }

    rows_removed(removed_rows);
    rows_added(n_added);
  }


}
