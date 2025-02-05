#include "listitem/track.hpp"

ListItem::Track::Track(const QStringList &strings,
	const lib::spt::track &track,
	const QIcon &icon,
	int index)
	: QTreeWidgetItem(strings)
{
	setIcon(0, icon);

	auto addedAt = QDateTime::fromString(QString::fromStdString(track.added_at),
		Qt::DateFormat::ISODate);

	setData(0, RoleTrack, QVariant::fromValue(track));
	setData(0, RoleIndex, index);
	setData(0, RoleAddedDate, addedAt);
	setData(0, RoleLength, track.duration);

	if (track.is_local || !track.is_playable)
	{
		setDisabled(true);
		setToolTip(1, track.is_local
			? "Local track"
			: "Unavailable");
	}

	// Artist
	auto names = lib::spt::entity::combine_names(track.artists, "\n");
	setToolTip(2, QString::fromStdString(names));

	// Title, album
	for (auto i = 1; i < strings.length() - 2; i++)
	{
		if (toolTip(i).isEmpty())
		{
			setToolTip(i, strings.at(i));
		}
	}

	// Length
	auto length = strings.at(strings.length() - 2).split(':');
	if (length.length() >= 2)
	{
		setToolTip(strings.length() - 2,
			QString("%1m %2s (%3s total)")
				.arg(length.at(0), length.at(1))
				.arg(track.duration / 1000));
	}

	// Added
	if (!DateUtils::isEmpty(addedAt))
	{
		setToolTip(strings.length() - 1,
			QLocale().toString(addedAt.date()));
	}
}

auto ListItem::Track::operator<(const QTreeWidgetItem &item) const -> bool
{
	auto column = treeWidget()->sortColumn();

	// Track number
	if (column == 0)
	{
		return data(0, RoleIndex).toInt() < item.data(0, RoleIndex).toInt();
	}

	// Length
	if (column == 4)
	{
		return data(0, RoleLength).toInt() < item.data(0, RoleLength).toInt();
	}

	// Added
	if (column == 5)
	{
		return data(0, RoleAddedDate).toDateTime() < item.data(0, RoleAddedDate).toDateTime();
	}

	return removePrefix(text(column))
		.compare(removePrefix(item.text(column)),
			Qt::CaseInsensitive) < 0;
}

auto ListItem::Track::removePrefix(const QString &str) -> QString
{
	return str.startsWith("The ", Qt::CaseInsensitive)
		? str.right(str.length() - 4)
		: str;
}
