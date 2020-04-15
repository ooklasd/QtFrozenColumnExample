/****************************************************************************
  **
  ** Copyright (C) 2016 The Qt Company Ltd.
  ** Contact: https://www.qt.io/licensing/
  **
  ** This file is part of the examples of the Qt Toolkit.
  **
  ** $QT_BEGIN_LICENSE:BSD$
  ** Commercial License Usage
  ** Licensees holding valid commercial Qt licenses may use this file in
  ** accordance with the commercial license agreement provided with the
  ** Software or, alternatively, in accordance with the terms contained in
  ** a written agreement between you and The Qt Company. For licensing terms
  ** and conditions see https://www.qt.io/terms-conditions. For further
  ** information use the contact form at https://www.qt.io/contact-us.
  **
  ** BSD License Usage
  ** Alternatively, you may use this file under the terms of the BSD license
  ** as follows:
  **
  ** "Redistribution and use in source and binary forms, with or without
  ** modification, are permitted provided that the following conditions are
  ** met:
  **   * Redistributions of source code must retain the above copyright
  **     notice, this list of conditions and the following disclaimer.
  **   * Redistributions in binary form must reproduce the above copyright
  **     notice, this list of conditions and the following disclaimer in
  **     the documentation and/or other materials provided with the
  **     distribution.
  **   * Neither the name of The Qt Company Ltd nor the names of its
  **     contributors may be used to endorse or promote products derived
  **     from this software without specific prior written permission.
  **
  **
  ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
  **
  ** $QT_END_LICENSE$
  **
  ****************************************************************************/

  #include "freezetablewidget.h"

  #include <QScrollBar>
  #include <QHeaderView>

  FreezeTableWidget::FreezeTableWidget(QWidget *parent)
	:QTableView(parent)
  {
        setObjectName("FreezeTableWidget");

	frozenTableView = new QTableView(this);
	frozenTableView->setObjectName("frozenTableView");
	frozenTableView->setVisible(false);
	//frozenTableView->setFocusPolicy(Qt::NoFocus);
	frozenTableView->setFocusProxy(this);
	frozenTableView->verticalHeader()->hide();
	frozenTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	frozenTableView->setStyleSheet(R"(
		QTableView { 
			border: none;
		}
	)");
	viewport()->stackUnder(frozenTableView);
	frozenTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	frozenTableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	setHorizontalScrollMode(ScrollPerPixel);
	setVerticalScrollMode(ScrollPerPixel);
	frozenTableView->setVerticalScrollMode(ScrollPerPixel);


	//connect the headers and scrollbars of both tableviews together
	connect(horizontalHeader(), &QHeaderView::sectionResized, this,
		&FreezeTableView::updateSectionWidth);
	connect(verticalHeader(), &QHeaderView::sectionResized, this,
		&FreezeTableView::updateSectionHeight);

	connect(frozenTableView->verticalScrollBar(), &QAbstractSlider::valueChanged,
		verticalScrollBar(), &QAbstractSlider::setValue);
	connect(verticalScrollBar(), &QAbstractSlider::valueChanged,
		frozenTableView->verticalScrollBar(), &QAbstractSlider::setValue);

  }

  FreezeTableWidget::~FreezeTableWidget()
  {
        delete frozenTableView;
  }

  void FreezeTableWidget::init()
  {
        frozenTableView->setModel(model());
	frozenTableView->setSelectionModel(selectionModel());
	frozenTableView->setSelectionMode(selectionMode());
	frozenTableView->setSelectionBehavior(selectionBehavior());
	for (int col = getFixColumn() + 1; col < model()->columnCount(); ++col)
		frozenTableView->setColumnHidden(col, true);

	for (size_t col = 0, Length = getFixColumn() + 1; col < Length; ++col)
	{
		frozenTableView->setColumnWidth(col, columnWidth(col));
	}

	frozenTableView->setVisible(getFixColumn() >= 0);

	updateFrozenTableGeometry();
  }

  void FreezeTableWidget::updateSectionWidth(int logicalIndex, int /* oldSize */, int newSize)
  {
        if (logicalIndex <= getFixColumn()){
		frozenTableView->setColumnWidth(logicalIndex, newSize);
		updateFrozenTableGeometry();
	}
  }

  void FreezeTableWidget::updateSectionHeight(int logicalIndex, int /* oldSize */, int newSize)
  {
        frozenTableView->setRowHeight(logicalIndex, newSize);
  }

  void FreezeTableWidget::resizeEvent(QResizeEvent * event)
  {
        QTableView::resizeEvent(event);
        updateFrozenTableGeometry();
   }

  QModelIndex FreezeTableWidget::moveCursor(CursorAction cursorAction,
                                            Qt::KeyboardModifiers modifiers)
  {
        QModelIndex current = QTableView::moveCursor(cursorAction, modifiers);

        if (cursorAction == MoveLeft && current.column() > 0
                && visualRect(current).topLeft().x() < frozenTableView->columnWidth(0) ){
              const int newValue = horizontalScrollBar()->value() + visualRect(current).topLeft().x()
                                   - frozenTableView->columnWidth(0);
              horizontalScrollBar()->setValue(newValue);
        }
        return current;
  }

  void FreezeTableWidget::scrollTo (const QModelIndex & index, ScrollHint hint){
      if (index.column() > 0)
          QTableView::scrollTo(index, hint);
  }

  void FreezeTableWidget::updateFrozenTableGeometry()
  {
        int columnsWidth = 0;
	for (size_t col = 0, Length = getFixColumn() + 1; col < Length; ++col)
	{
		columnsWidth += columnWidth(col);
	}

	frozenTableView->setGeometry(verticalHeader()->width() + frameWidth(),
		frameWidth(), columnsWidth,
		viewport()->height() + horizontalHeader()->height());
  }
