// Dropdowns that work on the Workshop build of Dabs Framework.
//
// DayZ 1.30 stopped compiling a template that, while it is still being instantiated, calls
// back into the template that instantiated it. DropdownListPrefab<T> creates
// DropdownListPrefabItem<T>, and the item called DropdownListPrefab<T>.DropdownElementExecute
// to report a click -- exactly that cycle. The Workshop Dabs Framework got past the compile
// error by commenting the call out:
//
//     if (dropdown) {
//         //dropdown.DropdownElementExecute(this);
//     }
//
// so clicking an option closes nothing and selects nothing: every DropdownListPrefab is
// stuck on its starting value.
//
// These two classes put the call back. Neither is a template, so the item names its
// dropdown by a plain class name and there is no cycle. Use UGDropdownListPrefab exactly
// like DropdownListPrefab<int>: same constructor, same `dropdown["Label"] = value`.
// It also works on a Dabs build that has the fix (the item then reports the click once,
// through this override, instead of through the base).
//
// The names carry the UG prefix on purpose: other plugins ship their own copy, and two
// mods declaring the same class would not load together.

class UGDropdownListPrefabItem: DropdownListPrefabItem<int>
{
	override bool DropdownElementExecute(ButtonCommandArgs args)
	{
		UGDropdownListPrefab dropdown = UGDropdownListPrefab.Cast(GetParent());
		if (dropdown) {
			dropdown.DropdownElementExecute(this);
		}

		return true;
	}
}

class UGDropdownListPrefab: DropdownListPrefab<int>
{
	// Same as DropdownListPrefab.Set, but builds the item that reports its click.
	override void Set(string item_text, int user_data)
	{
		UGDropdownListPrefabItem element = new UGDropdownListPrefabItem(item_text, user_data);
		element.SetParent(this);

		if (user_data == m_DefaultValue) {
			SetActiveListItem(element);
		}

		m_DropdownPrefabController.ItemList.Insert(element);
	}
}
