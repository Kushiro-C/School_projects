package com.example.mobile_project

import androidx.room.ColumnInfo
import androidx.room.Entity
import androidx.room.PrimaryKey

@Entity
data class Plant(
    @PrimaryKey(autoGenerate = true)
    var idPlant : Int = 0,
    @ColumnInfo(name = "nom") val nom: String?, // common name
    @ColumnInfo(name = "latin") val latin: String?,
    @ColumnInfo(name = "frequence") val frequence : Int, // normal frequency
    @ColumnInfo(name = "frequence2") val frequence2 : Int?, // season frequency
    @ColumnInfo(name = "season_from") val season_from : String?,
    @ColumnInfo(name = "season_to") val season_to : String?,
    @ColumnInfo(name = "frequence3") val frequence3 : Int?, // frequency with nutrients
    @ColumnInfo(name = "nutrient_from") val nutrient_from : String?,
    @ColumnInfo(name = "nutrient_to") val nutrient_to : String?,
    @ColumnInfo(name = "last_watering") var last_watering : String,
    @ColumnInfo(name = "image") var image : String? // image path in gallery (or @drawable/no_image)
)
