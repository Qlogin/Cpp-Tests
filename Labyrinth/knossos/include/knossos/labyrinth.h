/*!
\file
\brief Главный загаловочный файл библиотеки Knossos
*/

#pragma once

#include <knossos/export.h>

#include <boost/range/any_range.hpp>
#include <boost/optional.hpp>

#include <memory>
#include <exception>


namespace knossos
{
   /// Координаты на двухмерной плоскости
   struct position_t
   {
      position_t(int x = 0, int y = 0)
         : x(x)
         , y(y)
      {}

      int x, y;
   };

   /// Шагать можно только по следующим направлениям:
   enum direction_t
   {
      dir_up,     ///< вверх
      dir_left,   ///< влево
      dir_down,   ///< вниз
      dir_right,  ///< вправо

      /// Количество доступных направлений
      total_num
   };

   template <class Value, class Tag = boost::bidirectional_traversal_tag>
   struct values_range_t
   {
      typedef
         boost::any_range<const Value, Tag, const Value>
         type;
   };

   /// Последовательность направлений, задающая маршрут
   typedef
      values_range_t<direction_t>::type
      directions_range_t;

   /// Последовательность двухмерных коорлдинат, задающая секции лабиринта
   typedef
      values_range_t<position_t>::type
      positions_range_t;

   ////////////////////////////////////////////////////////////////////////////

   /*!
    * \brief Исключение, которое может кидать класс labyrinth_t
    */
   struct position_error_t : std::exception
   {
   };

   /*!
    *  \brief Класс, реализующий функционал "навигации по лабиринту"
    *
    *  Класс позволяет хранинить карту лабиринта в виде набора
    *  секций на двухмерной плоскости и текущую позицию внутри лабиринта.
    */
   class KNOSSOS_EXPORT labyrinth_t
   {
   public:
      /*!
       * \brief Конструкто по-умолчанию
       *
       * Задаёт лабиринт без секций. Текущая позиция не задана
       */
      labyrinth_t();
      ~labyrinth_t();

      /*!
       * \brief Создание лабиринта с помощью конструктора
       * \param sections последовательность координат секций лабиринта
       * \param start_position текущее положение
       * \throw position_error_t если секция с координатами текущего
       *                         положения отсутствует в последовательности
       */
      labyrinth_t(positions_range_t sections,
                  boost::optional<position_t> const & start_position = boost::none);

      /*!
       * \brief Добавляет новые секции в лабиринт
       * \param sections координаты
       */
      void add_sections(positions_range_t sections);

      /*!
       * \brief Удаляет секций из лабиринта
       * \param sections
       *
       * Если удаляется секция с координатами текущей позиции,
       * то позиция становится незаданной
       */
      void remove_sections(positions_range_t sections);

      /*!
       * \brief Возвращает координаты секций
       * \return
       */
      positions_range_t sections() const;

      /*!
       * \brief Позволяет узнать, задана ли текущая позция внутри лабиринта
       * \return true eсли позиция задана, иначе false
       */
      bool is_position_set() const;

      /*!
       * \brief Пытается установить текущую позицию в лабиринте
       * \param position координаты нового положение в либиринте
       * \return true если секция с заданными координатами существует
       *
       * Если секция с заданными координатами отсутствует в лабиринте,
       * то позиция остаётся прежней (в том числе и незаданной), а метод
       * возвращает значение false
       */
      bool set_position(position_t const & position);

      /*!
       * \brief Возрващает текущую позицию в лабиринте
       * \return координаты внутри лабиринта
       * \throw position_error_t в случае, если позиция не задана
       */
      position_t const & position() const;

      /*!
       * \brief Навигация по маршруту
       * \param route последовательность направлений
       * \param start_position начальные координаты маршрута
       * \return конечная точка маршрута
       * \throw position_error_t если начальные координаты не заданы (boost::none)
       *                         и текущее положение также не задано
       * \throw position_error_t если секция с заданными начальными координатами
       *                         отсутсвует в лабиринте
       *
       * Данный метод изменяет текущее положение в лабиринте на конечную
       * точку маршрута. Если в качестве start_position указать boost::none,
       * то начальной точкой маршщрута будет текущая позиция в лабиринте
       */
      position_t const & navigate(directions_range_t route,
         boost::optional<position_t> const & start_position = boost::none);

   private:
      struct impl_t;
      std::unique_ptr<impl_t> pimpl_;
   };
}
