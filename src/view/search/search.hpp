#pragma once

#include "lib/spotify/api.hpp"
#include "lib/httpclient.hpp"

#include "enum/searchtab.hpp"
#include "view/search/tracks.hpp"
#include "view/search/artists.hpp"
#include "view/search/albums.hpp"
#include "view/search/playlists.hpp"
#include "view/search/library.hpp"

#include <QDockWidget>
#include <QLineEdit>
#include <QListWidget>
#include <QTabWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

namespace View
{
	namespace Search
	{
		class Search: public QWidget
		{
		Q_OBJECT

		public:
			Search(lib::spt::api &spotify, lib::cache &cache,
				const lib::http_client &httpClient, QWidget *parent);

		private:
			QTabWidget *tabs = nullptr;
			QLineEdit *searchBox = nullptr;
			lib::spt::api &spotify;
			lib::cache &cache;
			const lib::http_client &httpClient;

			View::Search::Tracks *tracks = nullptr;
			View::Search::Artists *artists = nullptr;
			View::Search::Albums *albums = nullptr;
			View::Search::Playlists *playlists = nullptr;
			View::Search::Library *library = nullptr;

			void search();

		protected:
			void showEvent(QShowEvent *event) override;
			void hideEvent(QHideEvent *event) override;

		private:
			QString searchText;

			void resultsLoaded(const lib::spt::search_results &results);

			void onIndexChanged(int index);
		};
	}
}
