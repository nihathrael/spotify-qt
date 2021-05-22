#include "tracklistmodel.hpp"

TrackListModel::TrackListModel(const lib::settings &settings, QObject *parent)
	: settings(settings),
	QAbstractListModel(parent)
{
	constexpr int emptyPixmapSize = 64;

	// Empty icon used as replacement for play icon
	QPixmap emptyPixmap(emptyPixmapSize, emptyPixmapSize);
	emptyPixmap.fill(Qt::transparent);
	emptyIcon = QIcon(emptyPixmap);
}

void TrackListModel::add(const std::vector<lib::spt::track> &items)
{
	beginInsertRows(QModelIndex(), rowCount(),
		static_cast<int>(rowCount() + tracks.size() - 1));
	lib::vector::append(tracks, items);
	endInsertRows();
}

void TrackListModel::remove(lib::spt::track &item)
{
	std::vector<lib::spt::track>::iterator iter;
	for (iter = tracks.begin(); iter != tracks.end(); iter++)
	{
		if (iter->id == item.id)
		{
			break;
		}
	}

	if (iter == tracks.end())
	{
		return;
	}
	auto index = static_cast<int>(std::distance(tracks.begin(), iter));
	beginRemoveRows(QModelIndex(), index, index);
	tracks.erase(iter);
	endRemoveRows();
}

void TrackListModel::clear()
{
	beginRemoveRows(QModelIndex(), 0,
		static_cast<int>(tracks.size() - 1));
	tracks.clear();
	endRemoveRows();
}

auto TrackListModel::rowCount(const QModelIndex &/*parent*/) const -> int
{
	return static_cast<int>(tracks.size());
}

auto TrackListModel::rowCount() const -> int
{
	return rowCount(QModelIndex());
}

auto TrackListModel::columnCount(const QModelIndex &/*parent*/) const -> int
{
	constexpr int columnCount = 6;
	return columnCount;
}

auto TrackListModel::data(const QModelIndex &index, int role) const -> QVariant
{
	auto row = index.row();
	auto col = static_cast<TrackListColumn>(index.column());

	if (row < 0 || row >= tracks.size())
	{
		return QVariant();
	}

	const auto &track = tracks.at(row);

	switch (role)
	{
		case Qt::DisplayRole:
			return displayRole(col, row);

		case Qt::DecorationRole:
			return emptyIcon;

		case TrackRoleTrack:
			return QVariant::fromValue(track);

		case TrackRoleIndex:
			return index.row();

		default:
			return QVariant();
	}
}

auto TrackListModel::data(const QModelIndex &index) const -> QVariant
{
	return data(index, Qt::DisplayRole);
}

auto TrackListModel::roleNames() const -> QHash<int, QByteArray>
{
	return {
		{TrackRoleTrack, "track"},
		{TrackRoleIndex, "index"},
	};
}

auto TrackListModel::headerData(int section, Qt::Orientation orientation,
	int role) const -> QVariant
{
	if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
	{
		return QVariant();
	}

	switch (static_cast<TrackListColumn>(section))
	{
		case TrackListColumn::Number:
			return settings.general.track_numbers == lib::context_all
				? QString("#")
				: QString();

		case TrackListColumn::Name:
			return QString("Title");

		case TrackListColumn::Artists:
			return QString("Artist");

		case TrackListColumn::Album:
			return QString("Album");

		case TrackListColumn::Duration:
			return QString("Length");

		case TrackListColumn::AddedAt:
			return QString("Added");

		default:
			return QVariant();
	}
}

auto TrackListModel::at(int index) -> const lib::spt::track &
{
	return tracks.at(index);
}

auto TrackListModel::displayRole(TrackListColumn column, int row) const -> QString
{
	const auto &track = tracks.at(row);

	if (column == TrackListColumn::Number)
	{
		// TODO: Slow (needs to be done for each row)?
		auto fieldWidth = static_cast<int>(std::to_string(tracks.size()).size());
		return settings.general.track_numbers == lib::context_all
			? QString("%1").arg(row + 1, fieldWidth)
			: QString();
	}

	if (column == TrackListColumn::Name)
	{
		return QString::fromStdString(track.name);
	}

	if (column == TrackListColumn::Artists)
	{
		auto names = lib::spt::entity::combine_names(track.artists);
		return QString::fromStdString(names);
	}

	if (column == TrackListColumn::Album)
	{
		return QString::fromStdString(track.album.name);
	}

	if (column == TrackListColumn::Duration)
	{
		auto time = lib::fmt::time(track.duration);
		return QString::fromStdString(time);
	}

	if (column == TrackListColumn::AddedAt)
	{
		if (track.added_at.empty())
		{
			return QString();
		}

		if (settings.general.relative_added)
		{
			return DateUtils::toRelative(track.added_at);
		}

		auto date = DateUtils::fromIso(track.added_at).date();
		return QLocale::system().toString(date, QLocale::ShortFormat);
	}

	return QString();
}

auto TrackListModel::trackIds() const -> std::vector<std::string>
{
	std::vector<std::string> items;
	items.reserve(tracks.size());

	for (const auto &track : tracks)
	{
		if (!track.is_valid())
		{
			continue;
		}
		items.push_back(lib::spt::api::to_uri("track", track.id));
	}

	return items;
}