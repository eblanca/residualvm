/* ResidualVM - A 3D game interpreter
 *
 * ResidualVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the AUTHORS
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "engines/stark/services/diary.h"

#include "engines/stark/services/stateprovider.h"

namespace Stark {

Diary::Diary() {
	clear();
}

Diary::~Diary() {}

void Diary::clear() {
	_diaryEntries.clear();
	_fmvEntries.clear();
	_hasUnreadEntries = false;
	_pageIndex = 0;
}

void Diary::addDiaryEntry(const Common::String &name) {
	_diaryEntries.push_back(name);
	_hasUnreadEntries = true;
}

void Diary::addFMVEntry(const Common::String &filename, const Common::String &title, int gameDisc) {
	if (!hasFMVEntry(filename)) {
		FMVEntry entry;
		entry.filename = filename;
		entry.title = title;
		entry.gameDisc = gameDisc;
		_fmvEntries.push_back(entry);
	}
}

bool Diary::hasFMVEntry(const Common::String &filename) const {
	for (uint i = 0; i < _fmvEntries.size(); i++) {
		if (_fmvEntries[i].filename == filename) {
			return true;
		}
	}

	return false;
}

void Diary::readStateFromStream(Common::SeekableReadStream *stream, uint32 version) {
	clear();

	if (version <= 6) {
		return; //Early save versions did not persist the diary
	}

	ResourceSerializer serializer(stream, nullptr, version);
	saveLoad(&serializer);
}

void Diary::writeStateToStream(Common::WriteStream *stream) {
	ResourceSerializer serializer(nullptr, stream, StateProvider::kSaveVersion);
	saveLoad(&serializer);
}

void Diary::saveLoad(ResourceSerializer *serializer) {
	// Diary entries
	serializer->syncArraySize(_diaryEntries);
	for (uint i = 0; i < _diaryEntries.size(); i++) {
		serializer->syncAsString32(_diaryEntries[i]);
	}

	// FMV entries
	serializer->syncArraySize(_fmvEntries);
	for (uint i = 0; i < _fmvEntries.size(); i++) {
		serializer->syncAsString32(_fmvEntries[i].filename);
		serializer->syncAsString32(_fmvEntries[i].title);
		serializer->syncAsUint32LE(_fmvEntries[i].gameDisc);
	}

	// TODO: Persist conversation entries

	// Misc
	serializer->syncAsByte(_hasUnreadEntries);
	serializer->syncAsUint32LE(_pageIndex);
}

} // End of namespace Stark